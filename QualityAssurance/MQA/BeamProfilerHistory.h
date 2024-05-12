#ifndef QUALITYASSURANCE_BEAMPROFILERHISTORY_H
#define QUALITYASSURANCE_BEAMPROFILERHISTORY_H

#include <QWidget>

#include "TimeSeriesGraph.h"
#include "LinePlot.h"
#include "BeamProfileSeries.h"
#include "QARepo.h"
#include "Histogram.h"

class MQA;


class BeamProfilerHistory : public QObject
{
    Q_OBJECT

public:
    BeamProfilerHistory(MQA* parent, std::shared_ptr<QARepo> repo);
    ~BeamProfilerHistory();

public slots:
    void FetchData();
    void RecalculateFlatness();

signals:

private slots:
    void DisplayModelessMessageBox(QString msg, bool auto_close, QMessageBox::Icon icon);
    void DisplayInfo(QString msg);
    void DisplayWarning(QString msg);
    void DisplayError(QString msg);
    void DisplayCritical(QString msg);

    void ShowBeamProfile(Axis axis, QDateTime timestamp);

private:
    void RegisterGraphs();
    void SetDefaults();
    void ConnectSignals();

    MQA* parent_;
    std::shared_ptr<QARepo> repo_;
    TimeSeriesGraph flatness_graph_;
    TimeSeriesGraph width90_graph_;
    TimeSeriesGraph penumbra_graph_;
    Histogram flatness_histogram_;
    Histogram width90_histogram_;
    Histogram penumbra_histogram_;
    bool message_active_ = false;
};

#endif
