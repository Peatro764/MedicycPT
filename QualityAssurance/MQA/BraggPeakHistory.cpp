#include "BraggPeakHistory.h"
#include "ShowBraggPeakDialog.h"
#include "ui_MQA.h"
#include "MQA.h"

#include <QMessageBox>
#include <QDebug>
#include <QStandardPaths>

BraggPeakHistory::BraggPeakHistory(MQA *parent, std::shared_ptr<QARepo> repo) :
    parent_(parent),
    repo_(repo),
    width50_graph_(parent_->ui()->b_bp_chartView_width),
    penumbra_graph_(parent_->ui()->b_bp_chartView_penumbra),
    parcours_graph_(parent_->ui()->b_bp_chartView_parcours),
    width50_histogram_(parent_->ui()->b_bp_customPlot_longeur, "Longeur (50%)", "", 3.1, 3.7, 20.0),
    penumbra_histogram_(parent_->ui()->b_bp_customPlot_penumbre, "Penumbre", "", 0.7, 1.1, 20.0),
    parcours_histogram_(parent_->ui()->b_bp_customPlot_parcours, "Parcours", "", 30.3, 31.1, 20.0)
{
    RegisterGraphs();
    ConnectSignals();
}

BraggPeakHistory::~BraggPeakHistory() {}

void BraggPeakHistory::RegisterGraphs() {
    width50_graph_.CreateNewSerie(Axis::Z);
    penumbra_graph_.CreateNewSerie(Axis::Z);
    parcours_graph_.CreateNewSerie(Axis::Z);
    width50_histogram_.Register("Roue");
    penumbra_histogram_.Register("Roue");
    parcours_histogram_.Register("Roue");
}

void BraggPeakHistory::ConnectSignals() {
    QObject::connect(&width50_graph_, &TimeSeriesGraph::PointClicked, this, [=](Axis axis, QDateTime timestamp) { (void)axis; ShowBraggPeak(timestamp); });
    QObject::connect(&penumbra_graph_, &TimeSeriesGraph::PointClicked, this, [=](Axis axis, QDateTime timestamp) { (void)axis; ShowBraggPeak(timestamp); });
    QObject::connect(&parcours_graph_, &TimeSeriesGraph::PointClicked, this, [=](Axis axis, QDateTime timestamp) { (void)axis; ShowBraggPeak(timestamp); });
}

void BraggPeakHistory::ShowBraggPeak(QDateTime timestamp) {
    try {
        BraggPeak bp = parent_->qa_repo()->GetBraggPeak(timestamp);
        ShowBraggPeakDialog dialog(parent_, bp);
        dialog.exec();
    }
    catch (std::exception& exc) {
        qWarning() << "BraggPeakHistory::ShowBraggPeak Exception thrown: " << exc.what();
        DisplayError(QString("Failed getting braggpeak: ") + QString::fromStdString(exc.what()));
    }
}

void BraggPeakHistory::DisplayModelessMessageBox(QString msg, bool auto_close, QMessageBox::Icon icon) {
    if (!message_active_) {
        message_active_ = true;
        qDebug() << "BraggPeakHistory::DisplayModelessMessageBox " << msg;
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

void BraggPeakHistory::DisplayInfo(QString msg) {
    DisplayModelessMessageBox(msg, true, QMessageBox::Information);
}

void BraggPeakHistory::DisplayWarning(QString msg) {
    DisplayModelessMessageBox(msg, true, QMessageBox::Warning);
}

void BraggPeakHistory::DisplayError(QString msg) {
    DisplayModelessMessageBox(msg, true, QMessageBox::Critical);
}

void BraggPeakHistory::DisplayCritical(QString msg) {
    DisplayModelessMessageBox(msg, false, QMessageBox::Critical);
}

void BraggPeakHistory::FetchData(QDate from, QDate to) {
    try {
        BraggPeakSeries series = parent_->qa_repo()->GetBraggPeakSeries(from, to);

        width50_graph_.SetTimeRange(QDateTime(from),
                                    QDateTime(to.addDays(1)));
        penumbra_graph_.SetTimeRange(QDateTime(from),
                                    QDateTime(to.addDays(1)));
        parcours_graph_.SetTimeRange(QDateTime(from),
                                    QDateTime(to.addDays(1)));

        width50_graph_.SetPoints(Axis::Z, series.w50());
        penumbra_graph_.SetPoints(Axis::Z, series.penumbra());
        parcours_graph_.SetPoints(Axis::Z, series.parcours());

        width50_histogram_.Clear();
        penumbra_histogram_.Clear();
        parcours_histogram_.Clear();
        width50_histogram_.SetData("Roue", series.w50values());
        penumbra_histogram_.SetData("Roue", series.penumbravalues());
        parcours_histogram_.SetData("Roue", series.parcoursvalues());
        parent_->ui()->b_bp_lineEdit_parcours_mean->setText(QString::number(parcours_histogram_.Mean("Roue"), 'f', 2));
        parent_->ui()->b_bp_lineEdit_parcours_stddev->setText(QString::number(parcours_histogram_.StdDev("Roue"), 'f', 2));
        parent_->ui()->b_bp_lineEdit_penumbre_mean->setText(QString::number(penumbra_histogram_.Mean("Roue"), 'f', 2));
        parent_->ui()->b_bp_lineEdit_penumbre_stddev->setText(QString::number(penumbra_histogram_.StdDev("Roue"), 'f', 2));
        parent_->ui()->b_bp_lineEdit_longeur_mean->setText(QString::number(width50_histogram_.Mean("Roue"), 'f', 2));
        parent_->ui()->b_bp_lineEdit_longeur_stddev->setText(QString::number(width50_histogram_.StdDev("Roue"), 'f', 2));
    }
    catch (std::exception& exc) {
        qWarning() << "BraggPeakHistory::FetchData Exception thrown: " << exc.what();
        DisplayError(QString("Failed fetching data from db: " + QString::fromStdString(exc.what())));
    }
}
