#ifndef MILLINGMACHINE_H
#define MILLINGMACHINE_H

#include "millingmachine_global.h"

#include <memory>
#include <QObject>
#include <QSettings>
#include <QString>

#include "SerialPortWriter.h"
#include "SerialPortReader.h"
#include "SocketWriter.h"
#include "SocketReader.h"
#include "Collimator.h"
#include "Compensateur.h"
#include "Modulateur.h"
#include "DataReader.h"

class MillingMachine : public QObject
{
    Q_OBJECT

public:
    MillingMachine();
    bool SendProgramOverSerialPort();
    bool SendProgramOverNetwork();
    void ReadOverSerialPort();
    void ReadOverNetwork();
    bool SendProgramToStdOut() const;
    bool SendProgramToFile(QString filename) const;
    bool CreateMinimalistCollimatorProgram();

    bool CreateProgram(const Compensateur& compensateur);
    bool CreateProgram(const Collimator& collimator);
    bool CreateProgram(const Modulateur& modulateur);
    void SetProgram(QString program) { program_content_ = program; }
    QString GetProgram() { return program_content_; }
    DataReader* GetSocketReader() { return &socket_reader_; }
    DataReader* GetSerialPortReader() { return &serialport_reader_; }

    int GetDossier() const;
    void Abort();

signals:
    void ProgramSent();
    void SendError(QString error);
    void ProgramRead(QString program);
    void ReadError(QString error);

private:
    QString CmdFNX(float pos);
    QString CmdFNY(float pos);
    QString CmdFNZ(float pos);
    QString CmdFNRadius(float pos);
    QString CmdFN(QString QX, float pos);
    QString CmdMoveToPointInPolarCoord(float radius, float angle, QString radius_comp, QString feed_rate, QString misc);
    QString CmdCircleCenter(float x_pos, float y_pos);
    QString CmdDefineStartSubProgram(QString number);
    QString CmdDefineEndSubProgram();
    QString CmdCallSubProgram(QString number);
    QString CmdRepeatCallSubProgram(QString number, QString n_repeats);
    QString CmdToolIncOneAxisMovement(QString axis, float pos, QString radius_comp, QString feed_rate, QString misc);
    QString CmdToolOneAxisMovement(QString axis, float pos, QString radius_comp, QString feed_rate, QString misc);
    QString CmdToolIncTwoAxisMovement(QString axis1, float pos1, QString axis2, float pos2, QString radius_comp, QString feed_rate, QString misc);
    QString CmdToolTwoAxisMovement(QString axis1, float pos1, QString axis2, float pos2, QString radius_comp, QString feed_rate, QString misc);
    QString CmdToolData(QString number, QString spindle_axis, QString spindle_speed);
    QString CmdDefineTool(QString number, QString length, QString radius);
    QString CmdBeginProgram(QString dossier);
    QString CmdEndProgram(QString dossier);
    QString CmdBlankForm(QString spindle_axis,
                         QString lower_x, QString lower_y, QString lower_z,
                         QString upper_x, QString upper_y, QString upper_z);



    QString CmdBegin(QString dossier) const;
    QString DoubleFormat(const double& number, const int& digits) const;
    QString MakeModulateurStep(double angle);

    QSettings settings_;
    SerialPortWriter serialport_writer_;
    SerialPortReader serialport_reader_;
    SocketWriter socket_writer_;
    SocketReader socket_reader_;
    QString program_content_ = "";
    int program_line_number_ = 0;
    QString line_ending_ = "\n";
};

#endif
