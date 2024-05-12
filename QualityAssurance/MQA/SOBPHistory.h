#ifndef QUALITYASSURANCE_SOBPHistory_H
#define QUALITYASSURANCE_SOBPHistory_H

#include <QWidget>

#include "TimeSeriesGraph.h"
#include "LinePlot.h"
#include "SOBPHistory.h"
#include "QARepo.h"
#include "Histogram.h"

class MQA;

class SOBPHistory : public QObject
{
    Q_OBJECT

public:
    SOBPHistory(MQA* parent, std::shared_ptr<QARepo> repo, int modulator);
    ~SOBPHistory();

public slots:
    void FetchData(QDate from, QDate to);

signals:

private slots:
    void DisplayModelessMessageBox(QString msg, bool auto_close, QMessageBox::Icon icon);
    void DisplayInfo(QString msg);
    void DisplayWarning(QString msg);
    void DisplayError(QString msg);
    void DisplayCritical(QString msg);

    void ShowSOBP(QDateTime timestamp);

private:
    void ConnectSignals();
    void RegisterGraphs();

    MQA* parent_;
    std::shared_ptr<QARepo> repo_;
    TimeSeriesGraph penumbra_graph_;
    TimeSeriesGraph parcours_graph_;
    TimeSeriesGraph modulation_graph_;
    Histogram modulation_histogram_;
    Histogram penumbra_histogram_;
    Histogram parcours_histogram_;
    bool message_active_ = false;
    const int modulator_ = 10000;
};

#endif
