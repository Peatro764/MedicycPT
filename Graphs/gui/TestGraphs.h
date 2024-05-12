#ifndef QA_TESTGRAPHS_H
#define QA_TESTGRAPHS_H

#include <QObject>
#include <QWidget>
#include <QTimer>
#include <QFile>

#include <memory>
#include <map>
#include <queue>
#include <QObject>
#include <QDebug>

#include "Histogram.h"
#include "TimedStampedDataGraph.h"

namespace Ui {
class TestGraphs;
}

class TestGraphs : public QWidget
{
    Q_OBJECT

public:
    explicit TestGraphs(QWidget *parent = 0);
    ~TestGraphs();

public slots:

private slots:
    void AddValue1();
    void AddValue2();
    void AddValue3();
    void Normalize();

private:
    void SetupGraphs();
    void ConnectSignals();
    Ui::TestGraphs *ui_;
    std::shared_ptr<Histogram> histo_;
    std::shared_ptr<TimedStampedDataGraph> time_series_;
};

#endif
