#include "SOBPHistory.h"
#include "ShowSOBPDialog.h"
#include "ui_MQA.h"
#include "MQA.h"

#include <QMessageBox>
#include <QDebug>
#include <QStandardPaths>

SOBPHistory::SOBPHistory(MQA *parent, std::shared_ptr<QARepo> repo, int modulator) :
    parent_(parent),
    repo_(repo),
    penumbra_graph_(parent_->ui()->b_sobp_chartView_penumbra),
    parcours_graph_(parent_->ui()->b_sobp_chartView_parcours),
    modulation_graph_(parent_->ui()->b_sobp_chartView_modulation),
    modulation_histogram_(parent_->ui()->b_sobp_customPlot_modulation, "Modulation", "", 14.4, 15.4, 20.0),
    penumbra_histogram_(parent_->ui()->b_sobp_customPlot_penumbra, "Penumbre", "", 0.7, 1.3, 20.0),
    parcours_histogram_(parent_->ui()->b_sobp_customPlot_parcours, "Parcours", "", 30.0, 30.8, 20.0),
    modulator_(modulator)
{
    RegisterGraphs();
    ConnectSignals();
}

SOBPHistory::~SOBPHistory() {}

void SOBPHistory::RegisterGraphs() {
    penumbra_graph_.CreateNewSerie(Axis::Z);
    parcours_graph_.CreateNewSerie(Axis::Z);
    modulation_graph_.CreateNewSerie(Axis::Z);
    modulation_histogram_.Register("Roue");
    penumbra_histogram_.Register("Roue");
    parcours_histogram_.Register("Roue");
}

void SOBPHistory::ConnectSignals() {
//    QObject::connect(parent_->ui()->b_sobp_pushButton_results_updatePlot, &QPushButton::clicked, this, &SOBPHistory::FetchData);
    QObject::connect(&penumbra_graph_, &TimeSeriesGraph::PointClicked, this, [=](Axis axis, QDateTime timestamp) { (void)axis; ShowSOBP(timestamp); });
    QObject::connect(&parcours_graph_, &TimeSeriesGraph::PointClicked, this, [=](Axis axis, QDateTime timestamp) { (void)axis; ShowSOBP(timestamp); });
    QObject::connect(&modulation_graph_, &TimeSeriesGraph::PointClicked, this, [=](Axis axis, QDateTime timestamp) { (void)axis; ShowSOBP(timestamp); });
}

void SOBPHistory::ShowSOBP(QDateTime timestamp) {
    try {
        SOBP sobp = parent_->qa_repo()->GetSOBP(timestamp);
        ShowSOBPDialog dialog(parent_, sobp);
        dialog.exec();
    }
    catch (std::exception& exc) {
        qWarning() << "SOBPHistory::ShowSOBP Exception thrown: " << exc.what();
        DisplayError(QString("Failed getting sobp: ") + QString::fromStdString(exc.what()));
    }
}

void SOBPHistory::DisplayModelessMessageBox(QString msg, bool auto_close, QMessageBox::Icon icon) {
    if (!message_active_) {
        message_active_ = true;
        qDebug() << "SOBPHistory::DisplayModelessMessageBox " << msg;
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

void SOBPHistory::DisplayInfo(QString msg) {
    DisplayModelessMessageBox(msg, true, QMessageBox::Information);
}

void SOBPHistory::DisplayWarning(QString msg) {
    DisplayModelessMessageBox(msg, true, QMessageBox::Warning);
}

void SOBPHistory::DisplayError(QString msg) {
    DisplayModelessMessageBox(msg, true, QMessageBox::Critical);
}

void SOBPHistory::DisplayCritical(QString msg) {
    DisplayModelessMessageBox(msg, false, QMessageBox::Critical);
}

void SOBPHistory::FetchData(QDate from, QDate to) {
    try {
//        QDate from_date(parent_->ui()->b_sobp_dateEdit_from->date());
//        QDate to_date(parent_->ui()->b_sobp_dateEdit_to->date());
        SOBPSeries series = parent_->qa_repo()->GetSOBPSeries(modulator_, from, to);

        penumbra_graph_.SetTimeRange(QDateTime(from),
                                    QDateTime(to.addDays(1)));
        parcours_graph_.SetTimeRange(QDateTime(from),
                                    QDateTime(to.addDays(1)));
        modulation_graph_.SetTimeRange(QDateTime(from),
                                    QDateTime(to.addDays(1)));

        penumbra_graph_.SetPoints(Axis::Z, series.penumbra());
        parcours_graph_.SetPoints(Axis::Z, series.parcours());
        modulation_graph_.SetPoints(Axis::Z, series.modulation100());

        modulation_histogram_.Clear();
        penumbra_histogram_.Clear();
        parcours_histogram_.Clear();
        modulation_histogram_.SetData("Roue", series.modulationvalues());
        penumbra_histogram_.SetData("Roue", series.penumbravalues());
        parcours_histogram_.SetData("Roue", series.parcoursvalues());
        parent_->ui()->b_sobp_lineEdit_parcours_mean->setText(QString::number(parcours_histogram_.Mean("Roue"), 'f', 2));
        parent_->ui()->b_sobp_lineEdit_parcours_stddev->setText(QString::number(parcours_histogram_.StdDev("Roue"), 'f', 2));
        parent_->ui()->b_sobp_lineEdit_penumbra_mean->setText(QString::number(penumbra_histogram_.Mean("Roue"), 'f', 2));
        parent_->ui()->b_sobp_lineEdit_penumbra_stddev->setText(QString::number(penumbra_histogram_.StdDev("Roue"), 'f', 2));
        parent_->ui()->b_sobp_lineEdit_modulation_mean->setText(QString::number(modulation_histogram_.Mean("Roue"), 'f', 2));
        parent_->ui()->b_sobp_lineEdit_modulation_stddev->setText(QString::number(modulation_histogram_.StdDev("Roue"), 'f', 2));
    }
    catch (std::exception& exc) {
        qWarning() << "SOBPHistory::FetchData Exception thrown: " << exc.what();
        DisplayError(QString("Failed fetching data from db: " + QString::fromStdString(exc.what())));
    }
}
