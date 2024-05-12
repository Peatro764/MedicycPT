#ifndef MEDICYCPT_RADIATIONMONITOR_H
#define MEDICYCPT_RADIATIONMONITOR_H

#include <QWidget>
#include <QSettings>
#include <vector>
#include <QTimer>
#include <QMessageBox>
#include <QtDBus/QDBusConnection>

#include "RadiationLevelGraph.h"

#include "radiationmonitor_interface.h"

namespace Ui {
class RadiationMonitor;
}

class RadiationMonitor : public QWidget
{
    Q_OBJECT

public:
    explicit RadiationMonitor(QWidget *parent = 0);
    ~RadiationMonitor();

public slots:

private slots:
    void DisplayInfo(QString msg);
    void DisplayError(QString msg);

private:
    void SetupRadiationGraph();
    void SetupDBus();
    void ConnectSignals();
    Ui::RadiationMonitor *ui_;
    QSettings settings_;
    RadiationLevelGraph* radiation_level_graph_;
    medicyc::cyclotron::RadiationMonitorInterface* radiationmonitor_interface_;
    QString dbus_name_;
    bool message_active_ = false;
};

#endif
