#include "TotalDoseGraph.h"
#include "Calc.h"

#include <QDebug>

TotalDoseGraph::TotalDoseGraph(QtCharts::QChartView* chartview)
    : chartview_(chartview) {
    qDebug() << "TotalDoseGraph::TotalDoseGraph";
    dose_pie_.setHoleSize(0.25);
    dose_pie_.setPieSize(0.7);
    del_slice_ = dose_pie_.append("unk", 0.0);
    del_slice_->setColor(QColor("yellowgreen"));
    del_slice_->setLabelFont(QFont("Arial", 9));
    rem_slice_ = dose_pie_.append("unk", 100.0);
    rem_slice_->setLabelFont(QFont("Arial", 9));
    rem_slice_->setColor(QColor("cornflowerblue"));

    chartview_->setRenderHint(QPainter::Antialiasing);
    chartview_->chart()->setTitle("Cumul de dose");
    chartview_->chart()->setTitleFont(QFont("Century Schoolbook L", 16));
    chartview_->chart()->addSeries(&dose_pie_);
    chartview_->chart()->legend()->setVisible(true);
    chartview_->chart()->legend()->setAlignment(Qt::AlignBottom);
    chartview_->chart()->legend()->setFont(QFont("Arial", 10));
    chartview_->chart()->setDropShadowEnabled(true);

    SetInErrorCondition();
}

TotalDoseGraph::~TotalDoseGraph() {
    qDebug() << "TotalDoseGraph::~TotalDoseGraph";
    chartview_->chart()->removeSeries(&dose_pie_);
}

void TotalDoseGraph::SetData(const double& del_dose, const double& total_dose) {
    qDebug() << "TotalDoseGraph::SetData";
    if (del_dose < 0.0 || total_dose < 0.0) {
        qWarning() << "TotalDoseGraph::SetData Non valid input";
        SetInErrorCondition();
        return;
    }
    SetInProcess(del_dose, std::max(0.0, total_dose - del_dose));
}

void TotalDoseGraph::SetInProcess(const double &del_dose, const double &rem_dose) {
    del_slice_->setValue(del_dose);
    del_slice_->setExploded(false);
    del_slice_->setLabel(QString("Del: ") + QString::number(del_dose, 'f', 2) + " Gy");
    del_slice_->setLabelPosition(QtCharts::QPieSlice::LabelOutside);
    del_slice_->setLabelVisible(false);

    rem_slice_->setValue(rem_dose);
    rem_slice_->setLabel(QString("Res: ") + QString::number(rem_dose, 'f', 2) + " Gy");
    rem_slice_->setLabelPosition(QtCharts::QPieSlice::LabelOutside);
    rem_slice_->setLabelVisible(false);
}

void TotalDoseGraph::SetOverRadiation(const double &del_dose, const double &overshoot) {
    del_slice_->setValue(overshoot);
    del_slice_->setExploded(false);
    del_slice_->setColor(Qt::red);
    del_slice_->setLabel(QString("Sur: ") + QString::number(overshoot, 'f', 2) + " Gy");
    del_slice_->setLabelPosition(QtCharts::QPieSlice::LabelOutside);
    del_slice_->setLabelVisible(false);

    rem_slice_->setValue(del_dose);
    rem_slice_->setColor(QColor("yellowgreen"));
    rem_slice_->setLabel(QString("Res: ") + QString::number(del_dose, 'f', 2) +  " Gy");
    rem_slice_->setLabelPosition(QtCharts::QPieSlice::LabelOutside);
    rem_slice_->setLabelVisible(false);
}

void TotalDoseGraph::SetInErrorCondition() {
    del_slice_->setValue(0.0);
    del_slice_->setLabel("Unk");
    del_slice_->setExploded(false);
    del_slice_->setLabelVisible(false);

    rem_slice_->setValue(100.0);
    rem_slice_->setExploded(false);
    rem_slice_->setLabel("Unk");
    rem_slice_->setLabelPosition(QtCharts::QPieSlice::LabelOutside);
    rem_slice_->setLabelVisible(false);
}
