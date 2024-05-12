#ifndef MillingMachineGui_H
#define MillingMachineGui_H

#include <QWidget>
#include <QSettings>

#include "PTRepo.h"
#include "MillingMachine.h"
#include <QSystemTrayIcon>


namespace Ui {
class MillingMachineGui;
}

class MillingMachineGui : public QWidget
{
    Q_OBJECT

public:
    explicit MillingMachineGui(QWidget *parent = 0);
    ~MillingMachineGui();

public slots:
    void SetupDb();
    void SwitchToEthernetPort();
    void SwitchToSerialPort();
    void FillProgramTypes();
    void LaunchConfigDialog();
    void LaunchSaveFileDialog();
    void LaunchPrinterDialog();

    void ReadProgram();
    void SendProgram();
    void SendProgramToStdOut();
    void SaveProgramToDatabase();
    void ReadProgramFromDatabase();

    void CreateTestProgram();

    void WriteSuccess();
    void WriteError(QString error);

    void HandleNotification(QString channel, QSqlDriver::NotificationSource, QVariant message);

private slots:
    void TurnOffDbButton();
    void TurnOnDbButton();
    void FailedConnectingToDatabase();

private:
    void ShowAllMessages();
    void ClearMessages();
    void CreateActions();
    void CreateTrayIcon();
    QString ButtonStyleSheet(QString image);
    void UpdateProgram(QString id, QString type, QString program);
    Ui::MillingMachineGui *ui;
    MillingMachine milling_machine_;
    QSettings settings_;
    PTRepo *repo_;
    QStringList misc_types_;
    QStringList treatment_types_;

    bool serial_communication_;

    QSystemTrayIcon* tray_icon_;
    QMenu *tray_icon_menu_;
    QAction *show_msg_action_;
    QAction *clear_msg_action_;
    QAction *minimize_action_;
    QAction *maximize_action_;
    QAction *restore_action_;
    QAction *quit_action_;
    QStringList messages_;

    bool db_error_active_ = false;
};


#endif // MillingMachineGui_H
