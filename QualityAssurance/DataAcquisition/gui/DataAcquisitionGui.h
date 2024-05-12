#ifndef BEAMCHARACTERISTICS_DATAACQUISITIONGUI_H
#define BEAMCHARACTERISTICS_DATAACQUISITIONGUI_H

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

#include "Axis.h"
#include "Clock.h"
#include "IMotor.h"
#include "ISensor.h"
#include "Actuator.h"
#include "Range.h"
#include "QARepo.h"

#include "DevFactory.h"

using namespace t7;

enum class MeasurementType { PROFILE, BRAGGPEAK, CUVE, UNKNOWN };

namespace Ui {
class DataAcquisitionGui;
}

class DataAcquisitionGui : public QWidget
{
    Q_OBJECT

public:
    explicit DataAcquisitionGui(QWidget *parent = 0);
    ~DataAcquisitionGui();

public slots:

private slots:
    void SetupHardware();
    void MovementStarted(Axis axis, double pos);
    void MovementFinished(Axis axis, double pos);
    void MovementAborted(Axis axis, double pos);
    void CurrentPosition(Axis axis, double pos);
    void DisplayModelessMessageBox(QString msg);

private:
    void MovementMessage(Axis axis, double pos, QString message);
    std::map<SensorType, std::queue<double>> GetSensorMockData() const;
    void SetDefaults();
    void ConnectSignals();
    void FillComboBoxes();
    void SetupDb();
    Axis CurrentAxis();
    CLOCK CurrentClock();
    AIN CurrentAIN();
    AOUT CurrentAOUT();
    DIO CurrentDIO();
    PULSEOUT CurrentPulseOut();
    CLOCK CurrentPulseOutClock();

    Ui::DataAcquisitionGui *ui_;
    LabJack labjack_;
    std::shared_ptr<QARepo> repo_;
    std::shared_ptr<DevFactory> dev_factory_;
    QTime stopwatch_;
    QSettings settings_;

    std::map<Axis, std::shared_ptr<IMotor>> motors_;

};

#endif
