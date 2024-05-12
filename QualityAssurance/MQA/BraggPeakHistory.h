#ifndef QUALITYASSURANCE_BraggPeakHistory_H
#define QUALITYASSURANCE_BraggPeakHistory_H

#include <QWidget>

#include "TimeSeriesGraph.h"
//#include "LinePlot.h"
#include "BraggPeakHistory.h"
#include "QARepo.h"
#include "Histogram.h"

class MQA;

class BraggPeakHistory : public QObject
{
    Q_OBJECT

public:
    BraggPeakHistory(MQA* parent, std::shared_ptr<QARepo> repo);
    ~BraggPeakHistory();

public slots:
    void FetchData(QDate from, QDate to);

signals:

private slots:
    void DisplayModelessMessageBox(QString msg, bool auto_close, QMessageBox::Icon icon);
    void DisplayInfo(QString msg);
    void DisplayWarning(QString msg);
    void DisplayError(QString msg);
    void DisplayCritical(QString msg);

    void ShowBraggPeak(QDateTime timestamp);

private:
    void ConnectSignals();
    void RegisterGraphs();

    MQA* parent_;
    std::shared_ptr<QARepo> repo_;
    TimeSeriesGraph width50_graph_;
    TimeSeriesGraph penumbra_graph_;
    TimeSeriesGraph parcours_graph_;
    Histogram width50_histogram_;
    Histogram penumbra_histogram_;
    Histogram parcours_histogram_;
    bool message_active_ = false;
};

#endif
