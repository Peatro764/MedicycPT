#include "BeamProfilerHistory.h"
#include "ShowBeamProfileDialog.h"
#include "ui_MQA.h"
#include "MQA.h"

#include <QMessageBox>
#include <QDebug>
#include <QStandardPaths>

#include "BeamProfileResults.h"
#include "BeamProfileSeries.h"

BeamProfilerHistory::BeamProfilerHistory(MQA *parent, std::shared_ptr<QARepo> repo) :
    parent_(parent),
    repo_(repo),
    flatness_graph_(parent_->ui()->p_chartView_flatness),
    width90_graph_(parent_->ui()->p_chartView_l90),
    penumbra_graph_(parent_->ui()->p_chartView_penumbra),
    flatness_histogram_(parent_->ui()->p_customPlot_flatness, "Homogeniete", "", -4.0, 4.0, 40),
    width90_histogram_(parent_->ui()->p_customPlot_width90, "w90", "", 22.0, 24.0, 50),
    penumbra_histogram_(parent_->ui()->p_customPlot_penumbra, "Penumbre", "", 0.5, 3.0, 70)
{
    RegisterGraphs();
    ConnectSignals();
    SetDefaults();
}

BeamProfilerHistory::~BeamProfilerHistory() {}

void BeamProfilerHistory::RegisterGraphs() {
    flatness_graph_.CreateNewSerie(Axis::X);
    flatness_graph_.CreateNewSerie(Axis::Y);
    width90_graph_.CreateNewSerie(Axis::X);
    width90_graph_.CreateNewSerie(Axis::Y);
    penumbra_graph_.CreateNewSerie(Axis::X);
    penumbra_graph_.CreateNewSerie(Axis::Y);

    flatness_histogram_.Register("X");
    penumbra_histogram_.Register("X");
    width90_histogram_.Register("X");
    flatness_histogram_.Register("Y");
    penumbra_histogram_.Register("Y");
    width90_histogram_.Register("Y");
}

void BeamProfilerHistory::SetDefaults() {
    parent_->ui()->p_dateEdit_from->setDisplayFormat("yyyy.MM.dd");
    parent_->ui()->p_dateEdit_to->setDisplayFormat("yyyy.MM.dd");
    parent_->ui()->p_dateEdit_from->setCurrentSection(QDateTimeEdit::MonthSection);
    parent_->ui()->p_dateEdit_to->setCurrentSection(QDateTimeEdit::MonthSection);
    QDateTime current_date(QDateTime::currentDateTime());
    parent_->ui()->p_dateEdit_from->setDate(current_date.date().addDays(-182));
    parent_->ui()->p_dateEdit_to->setDate(current_date.date());
}

void BeamProfilerHistory::ConnectSignals() {
    QObject::connect(parent_->ui()->p_pushButton_results_updatePlot, &QPushButton::clicked, this, &BeamProfilerHistory::FetchData);
    QObject::connect(&flatness_graph_, &TimeSeriesGraph::PointClicked, this, &BeamProfilerHistory::ShowBeamProfile);
    QObject::connect(&width90_graph_, &TimeSeriesGraph::PointClicked, this, &BeamProfilerHistory::ShowBeamProfile);
    QObject::connect(&penumbra_graph_, &TimeSeriesGraph::PointClicked, this, &BeamProfilerHistory::ShowBeamProfile);
}

void BeamProfilerHistory::ShowBeamProfile(Axis axis, QDateTime timestamp) {
    try {
        BeamProfile profile = parent_->qa_repo()->GetBeamProfile(axis, timestamp);
        ShowBeamProfileDialog dialog(parent_, profile);
        dialog.exec();
    }
    catch (std::exception& exc) {
        qWarning() << "BeamProfilerHistory::ShowBeamProfile Exception thrown: " << exc.what();
        DisplayError(QString("Failed getting profile: ") + QString::fromStdString(exc.what()));
    }
}

void BeamProfilerHistory::DisplayModelessMessageBox(QString msg, bool auto_close, QMessageBox::Icon icon) {
    if (!message_active_) {
        message_active_ = true;
        qDebug() << "BeamProfilerHistory::DisplayModelessMessageBox " << msg;
        QMessageBox* box = new QMessageBox(parent_);
        box->setText(msg);
        box->setIcon(icon);
        box->setModal(false);
        box->setAttribute(Qt::WA_DeleteOnClose);
        QObject::connect(box, &QMessageBox::finished, this, [=]() { message_active_ = false; });
        if (auto_close) QTimer::singleShot(5000, box, SLOT(close()));
        box->show();
    }
}

void BeamProfilerHistory::DisplayInfo(QString msg) {
    DisplayModelessMessageBox(msg, true, QMessageBox::Information);
}

void BeamProfilerHistory::DisplayWarning(QString msg) {
    DisplayModelessMessageBox(msg, true, QMessageBox::Warning);
}

void BeamProfilerHistory::DisplayError(QString msg) {
    DisplayModelessMessageBox(msg, true, QMessageBox::Critical);
}

void BeamProfilerHistory::DisplayCritical(QString msg) {
    DisplayModelessMessageBox(msg, false, QMessageBox::Critical);
}


void BeamProfilerHistory::RecalculateFlatness() {
    QDate from_date(parent_->ui()->p_dateEdit_from->date());
    QDate to_date(parent_->ui()->p_dateEdit_to->date());
    qDebug() << "Fetching X profiles";
    std::vector<BeamProfile> profiles_x = parent_->qa_repo()->GetBeamProfiles(Axis::X, from_date, to_date);
    qDebug() << "Updating X";
    for (auto p : profiles_x) {
        parent_->qa_repo()->UpdateBeamProfile(p);
    }
    qDebug() << "Fetching Y profiles";
    std::vector<BeamProfile> profiles_y = parent_->qa_repo()->GetBeamProfiles(Axis::Y, from_date, to_date);
    qDebug() << "Updating Y";
    for (auto p : profiles_y) {
        parent_->qa_repo()->UpdateBeamProfile(p);
    }
    qDebug() << "Done";
}

void BeamProfilerHistory::FetchData() {
    try {
        QDate from_date(parent_->ui()->p_dateEdit_from->date());
        QDate to_date(parent_->ui()->p_dateEdit_to->date());
        BeamProfileSeries series_x = parent_->qa_repo()->GetBeamProfileSeries(Axis::X,
                                                                           from_date,
                                                                           to_date);
        BeamProfileSeries series_y = parent_->qa_repo()->GetBeamProfileSeries(Axis::Y,
                                                                           from_date,
                                                                           to_date);
        if (series_x.N() != series_y.N()) {
            qWarning() << "BeamProfilerHistory::FillResultVariationGraph Different number of x and y profiles in database";
        }

        width90_graph_.SetTimeRange(QDateTime(from_date),
                                    QDateTime(to_date.addDays(1)));
        flatness_graph_.SetTimeRange(QDateTime(from_date),
                                    QDateTime(to_date.addDays(1)));
        penumbra_graph_.SetTimeRange(QDateTime(from_date),
                                    QDateTime(to_date.addDays(1)));

        width90_graph_.SetPoints(Axis::X, series_x.w90());
        flatness_graph_.SetPoints(Axis::X, series_x.flatness());
        penumbra_graph_.SetPoints(Axis::X, series_x.penumbra());

        width90_graph_.SetPoints(Axis::Y, series_y.w90());
        flatness_graph_.SetPoints(Axis::Y, series_y.flatness());
        penumbra_graph_.SetPoints(Axis::Y, series_y.penumbra());

        flatness_histogram_.Clear();
        penumbra_histogram_.Clear();
        width90_histogram_.Clear();

        flatness_histogram_.SetData("X", series_x.flatnessvalues());
        flatness_histogram_.SetData("Y", series_y.flatnessvalues());
        penumbra_histogram_.SetData("X", series_x.penumbravalues());
        penumbra_histogram_.SetData("Y", series_y.penumbravalues());
        width90_histogram_.SetData("X", series_x.w90values());
        width90_histogram_.SetData("Y", series_y.w90values());

        parent_->ui()->p_lineEdit_x_flatness_mean->setText(QString::number(flatness_histogram_.Mean("X"), 'f', 2));
        parent_->ui()->p_lineEdit_x_flatness_sd->setText(QString::number(flatness_histogram_.StdDev("X"), 'f', 2));
        parent_->ui()->p_lineEdit_x_width90_mean->setText(QString::number(width90_histogram_.Mean("X"), 'f', 2));
        parent_->ui()->p_lineEdit_x_width90_sd->setText(QString::number(width90_histogram_.StdDev("X"), 'f', 2));
        parent_->ui()->p_lineEdit_x_penumbra_mean->setText(QString::number(penumbra_histogram_.Mean("X"), 'f', 2));
        parent_->ui()->p_lineEdit_x_penumbra_sd->setText(QString::number(penumbra_histogram_.StdDev("X"), 'f', 2));

        parent_->ui()->p_lineEdit_y_flatness_mean->setText(QString::number(flatness_histogram_.Mean("Y"), 'f', 2));
        parent_->ui()->p_lineEdit_y_flatness_sd->setText(QString::number(flatness_histogram_.StdDev("Y"), 'f', 2));
        parent_->ui()->p_lineEdit_y_width90_mean->setText(QString::number(width90_histogram_.Mean("Y"), 'f', 2));
        parent_->ui()->p_lineEdit_y_width90_sd->setText(QString::number(width90_histogram_.StdDev("Y"), 'f', 2));
        parent_->ui()->p_lineEdit_y_penumbra_mean->setText(QString::number(penumbra_histogram_.Mean("Y"), 'f', 2));
        parent_->ui()->p_lineEdit_y_penumbra_sd->setText(QString::number(penumbra_histogram_.StdDev("Y"), 'f', 2));

    }
    catch (std::exception& exc) {
        qWarning() << "BeamProfilerHistory::FillResultVariationGraph Exception thrown: " << exc.what();
        DisplayError(QString("Failed fetching data from db: " + QString::fromStdString(exc.what())));
    }
}
