#ifndef BEAMCHARACTERISTICS_STATEMACHINESGUI_H
#define BEAMCHARACTERISTICS_STATEMACHINESGUI_H

#include <QObject>
#include <QWidget>
#include <QTimer>
#include <QFile>

#include <memory>
#include <map>
#include <queue>
#include <QObject>
#include <QStateMachine>
#include <QTimer>
#include <QDebug>
#include <QSettings>

#include "IMotor.h"
#include "AxisScan.h"
#include "AxisStepper.h"
#include "DevFactory.h"
#include "Beacon.h"
#include "QARepo.h"
#include "Histogram.h"

namespace Ui {
class StateMachinesGui;
}

class StateMachinesGui : public QWidget
{
    Q_OBJECT

public:
    explicit StateMachinesGui(QWidget *parent = 0);
    ~StateMachinesGui();

public slots:

private slots:
    void ConfigureHardware();

private:
    QString VectorToString(const std::vector<double> data);
    void SetDefaults();
    void SetupDb();
    void FillComboBoxes();
    void ConnectSignals();
    Axis CurrentScanAxis();
    Axis CurrentStepAxis();
    Ui::StateMachinesGui *ui_;
    QSettings settings_;
    std::shared_ptr<QARepo> repo_;
    Beacon beacon_;
    std::shared_ptr<DevFactory> dev_factory_;
    std::map<Axis, std::shared_ptr<AxisScan>> axis_scanners_;
    std::map<Axis, std::shared_ptr<AxisStepper>> axis_steppers_;
    std::map<Axis, std::shared_ptr<IMotor>> motors_;
    std::shared_ptr<Histogram> position_accuracy_;

    QFile *file;
    QTextStream *stream;
};

#endif
