#include "MillingMachine.h"

#include <QSettings>
#include <QStandardPaths>
#include <QDebug>
#include <QTextStream>
#include <QFile>
#include <algorithm>

#include "SerialPortWriter.h"
#include "ModulateurMaker.h"

MillingMachine::MillingMachine() :
    settings_(QStandardPaths::locate(QStandardPaths::ConfigLocation, QString("MMM.ini"), QStandardPaths::LocateFile),
                                                          QSettings::IniFormat),
    serialport_writer_(settings_),
    serialport_reader_(settings_),
    socket_writer_(settings_),
    socket_reader_(settings_)

{
     QString line_ending_option(settings_.value("general/line_ending", "CRLF").toString());
     if (line_ending_option == QString("CRLF")) {
         line_ending_ = QString("\r\n");
         qDebug() << "Fraiseuese::MillingMachine CRLF line endings are used";
     } else if (line_ending_option == QString("LF")) {
         qDebug() << "Fraiseuese::MillingMachine LF line endings are used";
         line_ending_ = QString("\n");
     } else {
         qWarning() << "Fraiseuese::MillingMachine Unknown lineend option: " << line_ending_option << ", using default (LF)";
     }

     QObject::connect(&serialport_writer_, SIGNAL(WriteError(QString)), this, SIGNAL(SendError(QString)));
     QObject::connect(&serialport_writer_, SIGNAL(DataWritten()), this, SIGNAL(ProgramSent()));
     QObject::connect(&socket_writer_, SIGNAL(WriteError(QString)), this, SIGNAL(SendError(QString)));
     QObject::connect(&socket_writer_, SIGNAL(DataWritten()), this, SIGNAL(ProgramSent()));
}

void MillingMachine::Abort() {
    socket_writer_.Abort();
    socket_reader_.Abort();
    serialport_writer_.Abort();
}

int MillingMachine::GetDossier() const {
    auto first_line(program_content_.section("\n", 0, 1));
    auto words(first_line.split(" "));
    if (words.size() > 3) {
        return (words.at(3).toInt());
    } else {
        return 0;
    }
}

bool MillingMachine::SendProgramToFile(QString filename) const {
    QFile program_file(filename);

    if (!program_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "MillingMachine::SendProgramToFile Failed opening file";
        return false;
    }

    QTextStream stream(&program_file);
    stream << program_content_;
    program_file.close();

    return true;
}

bool MillingMachine::SendProgramToStdOut() const {
    if (program_content_.isEmpty()) {
        qWarning() << "MillingMachine::SendProgramToStdOut No program content";
        return false;
    }

    QTextStream stream(stdout);
    stream << program_content_;
    return true;
}

bool MillingMachine::SendProgramOverSerialPort() {
    if (program_content_.isEmpty()) {
        qWarning() << "MillingMachine::SendProgramOverSerialPort No program content";
        return false;
    }

    if (!serialport_writer_.Open()) {
        qWarning() << "MillingMachine::SendProgramOverSerialPort Failed opening port";
        emit(SendError(QString("Connection refused")));
        return false;
    }

    serialport_writer_.Write(program_content_.toUtf8());
    return true;
}

bool MillingMachine::SendProgramOverNetwork() {
    if (program_content_.isEmpty()) {
        qWarning() << "MillingMachine::SendProgramOverNetwork No program content";
        return false;
    }

    socket_writer_.Write(program_content_.toUtf8());
    return true;
}

void MillingMachine::ReadOverSerialPort() {
    if (!serialport_reader_.Open()) {
        qWarning() << "MillingMachine::ReadProgramOverSerialPort Failed opening port";
        emit(SendError(QString("Connection to serialport refused")));
        return;
    }

    serialport_reader_.Read();
}

void MillingMachine::ReadOverNetwork() {
    socket_reader_.Read();
}

bool MillingMachine::CreateMinimalistCollimatorProgram() {
    program_line_number_ = 0;
    program_content_ = QString("");
    QTextStream program_stream(&program_content_);
    program_stream << CmdBeginProgram("22345");
    program_stream << CmdEndProgram("22345");
    return true;
}

bool MillingMachine::CreateProgram(const Compensateur &compensateur) {
    if (!compensateur.IsValid()) {
        qWarning() << "MillingMachine::CreateProgram Compensateur is not valid";
        return false;
    }

    QCPCurveDataContainer coordinates(compensateur.FraiseuseFormat());
    if (coordinates.size() < 1) {
        qWarning() << "MillingMachine::CreateProgram Compensateur not coordinates";
        return false;
    }

    program_line_number_ = 0;
    program_content_ = QString("");
    QTextStream program_stream(&program_content_);
    program_stream << CmdBeginProgram(QString::number(compensateur.dossier()).mid(1));
    program_stream << program_line_number_++ << " FN 0: Q10 = +2.5" << line_ending_;
    program_stream << program_line_number_++ << " FN 0: Q11 = +26.5" << line_ending_;
    program_stream << program_line_number_++ << " FN 0: Q6 = +19" << line_ending_;
    program_stream << program_line_number_++ << " FN 0: Q7 = +20" << line_ending_;
    program_stream << CmdBlankForm("Z", "X-20", "Y-20", "Z-1", "X+20", "Y+20", "Z+26.5");
    program_stream << CmdDefineTool("1", "L+8,2", "R+6,97");
    program_stream << CmdDefineTool("2", "L+0", "R+Q10");
    program_stream << CmdDefineTool("3", "L-11,5", "R+1,5");
    program_stream << CmdToolData("1", "Z", "2000");
    program_stream << program_line_number_++ << " CYCL DEF 7.0 POINT ZERO" << line_ending_;
    program_stream << program_line_number_++ << " CYCL DEF 7.1 X+0" << line_ending_;
    program_stream << program_line_number_++ << " CYCL DEF 7.2 Y+0" << line_ending_;
    program_stream << program_line_number_++ << " CYCL DEF 7.3 Z+0" << line_ending_;
    program_stream << CmdToolTwoAxisMovement("X", 0.0, "Y", 0.0, "R0", "F MAX", "M13");
    program_stream << CmdCircleCenter(0.0, 0.0);
    program_stream << program_line_number_++ << " CYCL DEF 5.0 POCHE CIRC." << line_ending_;
    program_stream << program_line_number_++ << " CYCL DEF 5.1 DIST. -2" << line_ending_;
    double height_to_remove(26.5 - coordinates.at(0)->value - 1.0);
    program_stream << program_line_number_++ << " CYCL DEF 5.2 PROF. " << QString::number(-height_to_remove, 'f', 1) << line_ending_;
    program_stream << program_line_number_++ << " CYCL DEF 5.3 PASSE -5.5 F80" << line_ending_;
    program_stream << program_line_number_++ << " CYCL DEF 5.4 RAYON Q6" << line_ending_;
    program_stream << program_line_number_++ << " CYCL DEF 5.5 F100 DR+" << line_ending_;
    program_stream << CmdToolOneAxisMovement("Z", 28.5, "R0",  "F MAX", "M");
    program_stream << program_line_number_++ << " CYCL CALL M" << line_ending_;
    program_stream << program_line_number_++ << " FN 2: Q12 = +Q11 - +1.5" << line_ending_;
    program_stream << program_line_number_++ << " L Z+Q12 R0 F MAX M" << line_ending_;
    program_stream << program_line_number_++ << " LP PR+Q7 PA+0.000 RL F100 M" << line_ending_;
    program_stream << program_line_number_++ << " RND R2 F" << line_ending_;
    program_stream << program_line_number_++ << " CP IPA+360 DR+ RL F M" << line_ending_;
    program_stream << program_line_number_++ << " RND R2 F" << line_ending_;
    program_stream << CmdToolTwoAxisMovement("X", 0.0, "Y", 0.0, "R0", "F MAX", "M");
    program_stream << program_line_number_++ << " L R F M25" << line_ending_;
    program_stream << CmdToolData("2", "Z", "3000");
    program_stream << CmdToolTwoAxisMovement("X", 0.0, "Y", 0.0, "R0", "F MAX", "M13");
    const double init_pos_z = std::ceil(coordinates.at(0)->value + 1.0);
    program_stream << CmdToolOneAxisMovement("Z", init_pos_z, "R0",  "F MAX", "M");

    // Center point of compensateur
    program_stream << CmdFNX(compensateur.center_point().key);
    program_stream << CmdFNY(compensateur.center_point().value);

    program_stream << CmdDefineStartSubProgram("3");
    program_stream << program_line_number_++ << " L X+Q1 Y+Q2 R0 F MAX M13" << line_ending_;
    program_stream << program_line_number_++ << " CC X+Q1 Y+Q2" << line_ending_;
    program_stream << CmdToolOneAxisMovement("Z", init_pos_z, "R0",  "F MAX", "M");

    // Give pairs of radius / height (except first time where only height is given)
    program_stream << CmdFNZ(coordinates.at(0)->value);
    program_stream << program_line_number_++ << " FN 0: Q4 = +2" << line_ending_;
    program_stream << program_line_number_++ << " L Z+Q3 R0 F80 M" << line_ending_;
    program_stream << program_line_number_++ << " CALL LBL 1 REP" << line_ending_;
    for (int idx = 1; idx < coordinates.size(); ++idx) {
        if (coordinates.at(idx)->value < 0.0 || coordinates.at(idx)->key < 0.0) {
            qWarning() << "MillingMachine::CreateCompensateurProgram Compensateur has negative radius and/or height";
            return false;
        }
        program_stream << CmdFNZ(coordinates.at(idx)->value);
        program_stream << CmdFNRadius(coordinates.at(idx)->key);
        program_stream << program_line_number_++ << " CALL LBL 1 REP" << line_ending_;
    }

    program_stream << program_line_number_++ << " FN 11: IF +Q1 GT +1.5 GOTO LBL 2" << line_ending_;
    program_stream << program_line_number_++ << " FN 11: IF +Q2 GT +1.5 GOTO LBL 2" << line_ending_;
    program_stream << program_line_number_++ << " FN 12: IF +Q1 LT -1.5 GOTO LBL 2" << line_ending_;
    program_stream << program_line_number_++ << " FN 12: IF +Q2 LT -1.5 GOTO LBL 2" << line_ending_;
    program_stream << CmdCircleCenter(0.0, 0.0);
    program_stream << CmdToolOneAxisMovement("Z", 10, "R0",  "F MAX", "M");
    program_stream << CmdDefineStartSubProgram("4");
    program_stream << program_line_number_++ << " LP PR+Q6 PA+0 RL F80 M" << line_ending_;
    program_stream << program_line_number_++ << " CP IPA+360 DR+ RL F M" << line_ending_;
    program_stream << program_line_number_++ << " LP PR+Q5 PA+0 R0 F M" << line_ending_;

    program_stream << CmdDefineStartSubProgram("0");
    program_stream << CmdToolOneAxisMovement("Z", 5, "R0",  "F50", "M");
    program_stream << CmdCallSubProgram("4");
    program_stream << CmdToolOneAxisMovement("Z", 0, "R0",  "F50", "M");
    program_stream << CmdCallSubProgram("4");

    program_stream << CmdToolOneAxisMovement("Z", 30, "R0",  "F MAX", "M");
    program_stream << CmdRepeatCallSubProgram("3", "1");

    program_stream << program_line_number_++ << " L R F M25" << line_ending_;
    program_stream << CmdToolData("3", "Z", "3000");
    program_stream << CmdCircleCenter(0.0, 0.0);
    program_stream << CmdMoveToPointInPolarCoord(24.5f, 135.0f, "R0", "F MAX", "M13");
    program_stream << CmdToolOneAxisMovement("Z", 30, "R0",  "F MAX", "M");
    program_stream << CmdToolOneAxisMovement("Z", 22, "R0",  "F100", "M");
    program_stream << CmdMoveToPointInPolarCoord(17.5f, 135.0f, "R0", "F", "M");
    program_stream << CmdToolOneAxisMovement("Z", 30, "R0",  "F MAX", "M");

    program_stream << program_line_number_++ << " CYCL DEF 7.0 POINT ZERO" << line_ending_;
    program_stream << program_line_number_++ << " CYCL DEF 7.1 X+0" << line_ending_;
    program_stream << program_line_number_++ << " CYCL DEF 7.2 Y+0" << line_ending_;
    program_stream << program_line_number_++ << " CYCL DEF 7.3 Z+0" << line_ending_;

    program_stream << program_line_number_++ << " L R F M25" << line_ending_;
    program_stream << program_line_number_++ << " STOP M25" << line_ending_;
    program_stream << CmdDefineStartSubProgram("1");
    program_stream << program_line_number_++ << " FN 1: Q13 = +Q10 + +1" << line_ending_;
    program_stream << program_line_number_++ << " FN 1: Q5 = +Q4 + Q13" << line_ending_;
    program_stream << program_line_number_++ << " LP PR+Q5 PA+0 R0 F80 M" << line_ending_;
    program_stream << program_line_number_++ << " L Z+Q3 R F M" << line_ending_;
    program_stream << program_line_number_++ << " LP PR+Q4 PA+0 RL F M" << line_ending_;
    program_stream << program_line_number_++ << " RND R0,5 F" << line_ending_;
    program_stream << program_line_number_++ << " CP IPA+360 DR- RL F M" << line_ending_;
    program_stream << program_line_number_++ << " RND R0,5 F" << line_ending_;
    program_stream << program_line_number_++ << " LP PR+Q5 PA+0 R0 F M" << line_ending_;

    program_stream << CmdDefineStartSubProgram("0");
    program_stream << CmdEndProgram(QString::number(compensateur.dossier()).mid(1));

    return true;
}

bool MillingMachine::CreateProgram(const Collimator &collimator) {
    if (!collimator.IsValid()) {
        qWarning() << "MillingMachine::CreateProgram Collimator given in argument is not valid";
        return false;
    }

    program_line_number_ = 0;
    program_content_ = QString("");
    auto coordinates = collimator.InternalEdges();
    QTextStream program_stream(&program_content_);
    program_stream << CmdBeginProgram(QString::number(collimator.Dossier()));
    program_stream << CmdBlankForm("Z", "X-17", "Y-17", "Z-8", "X+17", "Y+17", "Z+0");
    program_stream << CmdDefineTool("1", "L+0", "R+1,60");
    program_stream << CmdDefineTool("2", "L+0", "R+1,50");
    program_stream << CmdDefineTool("3", "L+0", "R+1,50");
    program_stream << CmdToolData("1", "Z", "3000");
    program_stream << CmdToolTwoAxisMovement("X", 0.0, "Y", 0.0, "R0", "F MAX", "M03");
    program_stream << CmdToolOneAxisMovement("Z", 2.0, "R0",  "F MAX", "M");
    program_stream << CmdToolOneAxisMovement("Z", 0.0, "R0",  "F100",  "M");
    program_stream << CmdDefineStartSubProgram("1");
    program_stream << CmdToolIncOneAxisMovement("Z", -1.05f, "R0", "F100", "M");
    for (int idx = 0; idx < (int) coordinates.size(); ++idx) {
        QString radius_comp(idx == 0 ? "RL" : "R");
        program_stream << CmdToolTwoAxisMovement("X", -coordinates.at(idx).x(),
                                                 "Y", coordinates.at(idx).y(),
                                                 radius_comp, "F", "M");
    }
    program_stream << CmdToolTwoAxisMovement("X", 0.0f, "Y", 0.0f, "R0", "F", "M");
    program_stream << CmdDefineEndSubProgram();
    program_stream << CmdRepeatCallSubProgram("1", "7");
    program_stream << CmdToolData("2", "Z", "3000");
    program_stream << CmdToolOneAxisMovement("Z", -9.0f, "R0", "F MAX", "M03");
    program_stream << CmdCallSubProgram("1");
    program_stream << CmdToolData("3", "Z", "3000");
    program_stream << CmdToolOneAxisMovement("Z", -9.0f, "R0", "F MAX", "M03");
    program_stream << CmdCallSubProgram("1");
    program_stream << CmdToolOneAxisMovement("Z", 10.0f, "R0", "F MAX", "M");
    program_stream << CmdCircleCenter(0.0f, 0.0f);
    program_stream << CmdMoveToPointInPolarCoord(22.5f, 45.0f, "R0", "F MAX", "M03");
    program_stream << CmdToolOneAxisMovement("Z", 2.0f, "R0", "F MAX", "M");
    program_stream << CmdToolOneAxisMovement("Z", -2.0f, "R0", "F40", "M");
    program_stream << CmdMoveToPointInPolarCoord(18.5f, 45.0f, "R0", "F", "M");
    program_stream << CmdToolOneAxisMovement("Z", 2.0f, "R0", "F100", "M");
    program_stream << (program_line_number_++) << " L R F M25" << line_ending_; // TODO
    program_stream << CmdEndProgram(QString::number(collimator.Dossier()));
    return true;
}

bool MillingMachine::CreateProgram(const Modulateur& modulateur) {
    std::map<int, double>  full_angles(Modulateur::GetSectorAngularSizes(modulateur.um_plexi_weight(), modulateur.n_sectors()));
    const double max_thickness_mm(modulateur.steps() * modulateur.mod_step());
    if (modulateur.n_sectors() < 2 || modulateur.n_sectors() > 4) {
        qWarning() << "MillingMachine::CreateProgram Can only modulateurs with 2, 3 or 4 wings";
        return false;
    }
    if (((int)full_angles.size()) < 4) {
        qWarning() << "MillingMachine::CreateProgram Cannot create modulateur with less than 3 steps";
        return false;
    }

    auto it = full_angles.begin();
    ++it;
    const double first_angle(it->second / 2.0);
    ++it;
    const double second_angle(it->second / 2.0);
    std::vector<double> remaining_angles;
    ++it;
    for (auto it_rem = it; it_rem != full_angles.end(); ++it_rem) {
        remaining_angles.push_back(it_rem->second / 2.0);
    }

    const QString n_rot(QString::number(modulateur.n_sectors() - 1));
    QString drill_radius("NA");
    QString rot_angle("NA");
    QString inc_rot_angle("NA");
    QString todd_angle("NA");
    switch (modulateur.n_sectors()) {
    case 2:
        drill_radius = QString("+5.000");
        rot_angle = QString("+45.0");
        inc_rot_angle = QString("+180.0");
        todd_angle = QString("+77.0");
        break;
    case 3:
        drill_radius = QString("+3.000");
        rot_angle = QString("-15.0");
        inc_rot_angle = QString("+120.0");
        todd_angle = QString("+54.0");
        break;
    case 4:
        drill_radius = QString("+2.975");
        rot_angle = QString("+0.0");
        inc_rot_angle = QString("+90.0");
        todd_angle = QString("+38.5");
        break;
    default:
        qWarning() << "MillingMachine::CreateProgram Non valid n sectors";
        return false;
        break;
    }

    if (first_angle < 36.5) {
        drill_radius = DoubleFormat(std::max(drill_radius.toDouble(), 4.0), 3);
    }

    program_line_number_ = 0;
    program_content_ = QString("");
    QTextStream program_stream(&program_content_);
    program_stream << CmdBeginProgram(modulateur.id());
    program_stream << CmdBlankForm("Z", "X-95", "Y-95", "Z" + DoubleFormat(-max_thickness_mm, 1), "X+95", "Y+95", "Z+2");
    program_stream << (program_line_number_++) << " FN 0: Q1 = " << DoubleFormat(max_thickness_mm, 1) << line_ending_;
    program_stream << (program_line_number_++) << " FN 0: Q2 = " << drill_radius << line_ending_;
    program_stream << CmdDefineTool("1", "L+0", "R+Q2");
    program_stream << CmdDefineTool("2", "L+0", "R+1.5");
    program_stream << CmdToolData("1", "Z", "1800");
    program_stream << (program_line_number_++) << " CYCL DEF 10.0 ROTATION" << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 10.1 ROT" << rot_angle << line_ending_;
    program_stream << CmdToolTwoAxisMovement("X", 0.0, "Y", 0.0, "R0", "F MAX", "M13");
    program_stream << CmdCircleCenter(0.0, 0.0);
    program_stream << (program_line_number_++) << " FN 0: Q5 = " << DoubleFormat(first_angle, 3) << line_ending_;
    program_stream << (program_line_number_++) << " FN 12: IF +Q5 LT " << todd_angle << " GOTO LBL 6" << line_ending_;
    program_stream << (program_line_number_++) << " FN 9: IF +Q5 EQU " << todd_angle << " GOTO LBL 6" << line_ending_;
    program_stream << (program_line_number_++) << " LP PR+20 PA+Q5 RL F MAX M13" << line_ending_;
    program_stream << (program_line_number_++) << " FN 4: Q3 = -Q1 DIV +3" << line_ending_;
    program_stream << (program_line_number_++) << " FN 2: Q4 = -Q1 - +Q3" << line_ending_;
    program_stream << (program_line_number_++) << " FN 1: Q14 = -Q1 + +0.8" << line_ending_;
    program_stream << (program_line_number_++) << " L Z+10 RL F MAX M" << line_ending_;
    program_stream << (program_line_number_++) << " L Z+Q3 RL F180 M" << line_ending_;
    program_stream << (program_line_number_++) << " LBL 1" << line_ending_;
    program_stream << (program_line_number_++) << " LP PR+62 PA+Q5 RL F120 M" << line_ending_;
    program_stream << (program_line_number_++) << " L IZ-0.8 RL F M" << line_ending_;
    program_stream << (program_line_number_++) << " LP PR+94.5 PA+Q5 RL F120 M" << line_ending_;
    program_stream << (program_line_number_++) << " RND R2 F" << line_ending_;
    program_stream << (program_line_number_++) << " CP PA-Q5 DR- RL F M" << line_ending_;
    program_stream << (program_line_number_++) << " RND R2 F" << line_ending_;
    program_stream << (program_line_number_++) << " LP PR+62 PA-Q5 RL F M" << line_ending_;
    program_stream << (program_line_number_++) << " L IZ+0.8 RL F M" << line_ending_;
    program_stream << (program_line_number_++) << " LP PR+20 PA-Q5 RL F M" << line_ending_;
    program_stream << (program_line_number_++) << " L Z+10 RL F MAX M" << line_ending_;
    program_stream << (program_line_number_++) << " L X+0 Y+0 R0 F MAX M" << line_ending_;
    program_stream << (program_line_number_++) << " LP PR+20 PA+Q5 RL F MAX M" << line_ending_;
    program_stream << (program_line_number_++) << " LBL 0" << line_ending_;
    program_stream << (program_line_number_++) << " L Z+Q4 RL F120 M" << line_ending_;
    program_stream << (program_line_number_++) << " CALL LBL 1 REP" << line_ending_;
    program_stream << (program_line_number_++) << " L Z+Q14 RL F120 M" << line_ending_;
    program_stream << (program_line_number_++) << " CALL LBL 1 REP" << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 10.0 ROTATION" << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 10.1 IROT" << inc_rot_angle << line_ending_;
    program_stream << (program_line_number_++) << " LP PR+20 PA+Q5 RL F MAX M" << line_ending_;
    program_stream << (program_line_number_++) << " L Z+Q3 RL F120 M" << line_ending_;
    program_stream << (program_line_number_++) << " CALL LBL 1 REP " <<  n_rot << " /" << n_rot << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 10.0 ROTATION" << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 10.1 ROT" << rot_angle << line_ending_;
    program_stream << (program_line_number_++) << " FN 11: IF +Q5 GT " << todd_angle << " GOTO LBL 8" << line_ending_;
    program_stream << (program_line_number_++) << " LBL 6" << line_ending_;
    program_stream << (program_line_number_++) << " LP PR+20 PA+Q5 RL F MAX M13" << line_ending_;
    program_stream << (program_line_number_++) << " FN 4: Q3 = -Q1 DIV +3" << line_ending_;
    program_stream << (program_line_number_++) << " FN 2: Q4 = -Q1 - +Q3" << line_ending_;
    program_stream << (program_line_number_++) << " L Z+10 RL F MAX M" << line_ending_;
    program_stream << (program_line_number_++) << " L Z+Q3 RL F120 M" << line_ending_;
    program_stream << (program_line_number_++) << " LBL 7" << line_ending_;
    program_stream << (program_line_number_++) << " LP PR+94.5 PA+Q5 RL F120 M" << line_ending_;
    program_stream << (program_line_number_++) << " RND R2 F" << line_ending_;
    program_stream << (program_line_number_++) << " CP PA-Q5 DR- RL F M" << line_ending_;
    program_stream << (program_line_number_++) << " RND R2 F" << line_ending_;
    program_stream << (program_line_number_++) << " LP PR+20 PA-Q5 RL F M" << line_ending_;
    program_stream << (program_line_number_++) << " L Z+10 RL F MAX M" << line_ending_;
    program_stream << (program_line_number_++) << " L X+0 Y+0 R0 F MAX M" << line_ending_;
    program_stream << (program_line_number_++) << " LP PR+20 PA+Q5 RL F MAX M" << line_ending_;
    program_stream << (program_line_number_++) << " LBL 0" << line_ending_;
    program_stream << (program_line_number_++) << " L Z+Q4 RL F120 M" << line_ending_;
    program_stream << (program_line_number_++) << " CALL LBL 7 REP" << line_ending_;
    program_stream << (program_line_number_++) << " L Z-Q1 RL F120 M" << line_ending_;
    program_stream << (program_line_number_++) << " CALL LBL 7 REP" << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 10.0 ROTATION" << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 10.1 IROT" << inc_rot_angle << line_ending_;
    program_stream << (program_line_number_++) << " LP PR+20 PA+Q5 RL F MAX M" << line_ending_;
    program_stream << (program_line_number_++) << " L Z+Q3 RL F120 M" << line_ending_;
    program_stream << (program_line_number_++) << " CALL LBL 7 REP " << n_rot << " /" << n_rot << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 10.0 ROTATION" << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 10.1 ROT" << rot_angle << line_ending_;
    program_stream << (program_line_number_++) << " LBL 8" << line_ending_;
    program_stream << (program_line_number_++) << " L R F M25" << line_ending_;
    program_stream << (program_line_number_++) << " TOOL CALL 1 Z S 1800" << line_ending_;
    program_stream << (program_line_number_++) << " FN 0: Q20 = +100" << line_ending_;
    program_stream << (program_line_number_++) << " FN 0: Q21 = +130" << line_ending_;
    program_stream << (program_line_number_++) << " FN 0: Q22 = +180" << line_ending_;
    program_stream << (program_line_number_++) << " FN 0: Q6 = " << DoubleFormat(second_angle, 3) << line_ending_;
    program_stream << (program_line_number_++) << " FN 2: Q7 = +Q1 - +0.8" << line_ending_;
    program_stream << (program_line_number_++) << " LBL 10" << line_ending_;
    program_stream << (program_line_number_++) << " FN 2: Q8 = +Q5 - +Q6" << line_ending_;
    program_stream << (program_line_number_++) << " FN 4: Q9 = +Q8 DIV +2" << line_ending_;
    program_stream << (program_line_number_++) << " FN 2: Q10 = +Q5 - +Q9" << line_ending_;
    program_stream << (program_line_number_++) << " LBL 0" << line_ending_;
    program_stream << (program_line_number_++) << " LBL 5" << line_ending_;
    program_stream << (program_line_number_++) << " FN 7: Q11 = COS +Q6" << line_ending_;
    program_stream << (program_line_number_++) << " FN 4: Q12 = +Q2 DIV +Q11" << line_ending_;
    program_stream << (program_line_number_++) << " LBL 0" << line_ending_;
    program_stream << (program_line_number_++) << " LBL 2" << line_ending_;
    program_stream << (program_line_number_++) << " L Z+10 R0 F MAX M13" << line_ending_;
    program_stream << (program_line_number_++) << " CC X+0 Y+Q12" << line_ending_;
    program_stream << (program_line_number_++) << " LP PR+98 PA+Q10 R0 F MAX M13" << line_ending_;
    program_stream << (program_line_number_++) << " L Z-Q7 R0 FQ20 M" << line_ending_;

    program_stream << (program_line_number_++) << " LP PR+20 PA+Q6 R0 FQ21 M" << line_ending_;
    program_stream << (program_line_number_++) << " LP PR+98 IPA+0 R0 F M" << line_ending_;

    for (auto angle : remaining_angles) {
        program_stream << MakeModulateurStep(angle);
    }
    program_stream << MakeModulateurStep(0.0);
    program_stream << (program_line_number_++) << " FN 0: Q6 = " << DoubleFormat(second_angle, 3) << line_ending_;
    program_stream << (program_line_number_++) << " FN 0: Q5 = " << DoubleFormat(first_angle, 3) << line_ending_;
    program_stream << (program_line_number_++) << " CALL LBL 3 REP" << line_ending_;
    program_stream << (program_line_number_++) << " CALL LBL 10 REP" << line_ending_;

    program_stream << (program_line_number_++) << " LBL 0" << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 10.0 ROTATION" << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 10.1 IROT" << inc_rot_angle << line_ending_;
    program_stream << (program_line_number_++) << " CALL LBL 2 REP " << n_rot << " /" << n_rot << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 10.0 ROTATION" << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 10.1 ROT" << rot_angle << line_ending_;
    program_stream << (program_line_number_++) << " LBL 4" << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 8.0 IMAGE MIROIR" << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 8.1 Y" << line_ending_;
    program_stream << (program_line_number_++) << " CALL LBL 2 REP" << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 8.0 IMAGE MIROIR" << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 8.1" << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 10.0 ROTATION" << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 10.1 IROT" << inc_rot_angle << line_ending_;
    program_stream << (program_line_number_++) << " LBL 0" << line_ending_;
    program_stream << (program_line_number_++) << " CALL LBL 4 REP " << n_rot << " /" << n_rot << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 10.0 ROTATION" << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 10.1 ROT" << rot_angle << line_ending_;
    program_stream << (program_line_number_++) << " L Z+10 R0 F MAX M" << line_ending_;
    program_stream << (program_line_number_++) << " FN 2: Q13 = +Q1 - +10" << line_ending_;
    program_stream << (program_line_number_++) << " FN 12: IF +Q13 LT +0 GOTO LBL 20" << line_ending_;
    program_stream << (program_line_number_++) << " FN 9: IF +Q13 EQU +0 GOTO LBL 20" << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 5.0 POCHE CIRC." << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 5.1 DIST. -10" << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 5.2 PROF. -Q13" << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 5.3 PASSE -7 F120" << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 5.4 RAYON 25" << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 5.5 F120 DR+" << line_ending_;
    program_stream << (program_line_number_++) << " L X+0 Y+0 R0 F MAX M" << line_ending_;
    program_stream << (program_line_number_++) << " L Z+10 R0 F MAX M" << line_ending_;
    program_stream << (program_line_number_++) << " CYCL CALL M" << line_ending_;
    program_stream << (program_line_number_++) << " L R F M25" << line_ending_;
    program_stream << (program_line_number_++) << " FN 12: IF +Q5 LT " << todd_angle << " GOTO LBL 20" << line_ending_;
    program_stream << (program_line_number_++) << " FN 9: IF +Q5 EQU " << todd_angle << " GOTO LBL 20" << line_ending_;
    program_stream << (program_line_number_++) << " TOOL CALL 2 Z S 3000" << line_ending_;
    program_stream << (program_line_number_++) << " FN 2: Q17 = " + inc_rot_angle + " - +Q5" << line_ending_;
    program_stream << (program_line_number_++) << " LBL 9" << line_ending_;
    program_stream << (program_line_number_++) << " L X+0 Y+0 R0 F MAX M13" << line_ending_;
    program_stream << (program_line_number_++) << " CC X+0 Y+0" << line_ending_;
    program_stream << (program_line_number_++) << " LP PR+65 PA+Q5 RR F MAX M13" << line_ending_;
    program_stream << (program_line_number_++) << " L Z+0 RR F MAX M" << line_ending_;
    program_stream << (program_line_number_++) << " L Z-Q1 RR F180 M" << line_ending_;
    program_stream << (program_line_number_++) << " LP PR+20 PA+Q5 RR F M" << line_ending_;
    program_stream << (program_line_number_++) << " LP PR+20 PA+Q17 RR F M" << line_ending_;
    program_stream << (program_line_number_++) << " LP PR+65 PA+Q17 RR F M" << line_ending_;
    program_stream << (program_line_number_++) << " L Z+10 RR F MAX M" << line_ending_;
    program_stream << (program_line_number_++) << " L X+0 Y+0 R0 F MAX M" << line_ending_;
    program_stream << (program_line_number_++) << " LBL 0" << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 10.0 ROTATION" << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 10.1 IROT" << inc_rot_angle << line_ending_;
    program_stream << (program_line_number_++) << " CALL LBL 9 REP " << n_rot << " /" << n_rot << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 10.0 ROTATION" << line_ending_;
    program_stream << (program_line_number_++) << " CYCL DEF 10.1 ROT" << rot_angle << line_ending_;
    program_stream << (program_line_number_++) << " LBL 20" << line_ending_;
    program_stream << (program_line_number_++) << " STOP M25" << line_ending_;
    program_stream << (program_line_number_++) << " LBL 3" << line_ending_;
    program_stream << (program_line_number_++) << " L IZ+0.8 R0 FQ22 M13" << line_ending_;
    program_stream << (program_line_number_++) << " CALL LBL 5 REP" << line_ending_;
    program_stream << (program_line_number_++) << " CC X+0 Y+Q12" << line_ending_;
    program_stream << (program_line_number_++) << " LBL 0" << line_ending_;
    program_stream << (program_line_number_++) << " LBL 11" << line_ending_;
    program_stream << (program_line_number_++) << " LP PR+98 PA+Q10 R0 F MAX M13" << line_ending_;
    program_stream << (program_line_number_++) << " LP PR+20 PA+Q6 R0 FQ21 M" << line_ending_;
    program_stream << (program_line_number_++) << " LP PR+98 IPA+0 R0 F M" << line_ending_;
    program_stream << (program_line_number_++) << " FN 0: Q5 = +Q6" << line_ending_;
    program_stream << (program_line_number_++) << " LBL 0" << line_ending_;
    program_stream << (program_line_number_++) << " LBL 12" << line_ending_;
    program_stream << (program_line_number_++) << " CALL LBL 10 REP" << line_ending_;
    program_stream << (program_line_number_++) << " CALL LBL 3 REP" << line_ending_;
    program_stream << (program_line_number_++) << " CALL LBL 11 REP" << line_ending_;
    program_stream << (program_line_number_++) << " LBL 0" << line_ending_;
    program_stream << (program_line_number_++) << " L R F M25" << line_ending_;
    program_stream << CmdEndProgram(modulateur.id());

    return true;
}

QString MillingMachine::MakeModulateurStep(double angle/*, bool odd*/) {
    QString cmd("");
    QTextStream stream(&cmd);
    stream << (program_line_number_++) << " FN 0: Q6 = " << DoubleFormat(angle, 3) << line_ending_;
    stream << (program_line_number_++) << " CALL LBL 12 REP" << line_ending_;
    return cmd;
}

QString MillingMachine::CmdFNX(float pos) {
    return CmdFN("Q1", pos);
}

QString MillingMachine::CmdFNY(float pos) {
    return CmdFN("Q2", pos);
}

QString MillingMachine::CmdFNZ(float pos) {
    return CmdFN("Q3", pos);
}

QString MillingMachine::CmdFNRadius(float pos) {
    return CmdFN("Q4", pos);
}

QString MillingMachine::CmdFN(QString QX, float pos) {
    QString cmd("");
    QTextStream stream(&cmd);
    stream << (program_line_number_++);
    stream << " FN 0: " << QX << " = " << DoubleFormat(pos, 3) << line_ending_;
    return cmd;
}


QString MillingMachine::CmdMoveToPointInPolarCoord(float radius, float angle, QString radius_comp, QString feed_rate, QString misc) {
    QString cmd("");
    QTextStream stream(&cmd);
    stream << (program_line_number_++);
    stream << " LP PR" << DoubleFormat(radius, 3) << " PA" << DoubleFormat(angle, 3) << " " << radius_comp << " " << feed_rate << " " << misc << line_ending_;
    return cmd;
}

QString MillingMachine::CmdCircleCenter(float x_pos, float y_pos) {
    // No tool movement
    QString cmd("");
    QTextStream stream(&cmd);
    stream << (program_line_number_++);
    stream << " CC X" << DoubleFormat(x_pos, 3) << " Y" << DoubleFormat(y_pos, 3) << line_ending_;
    return cmd;
}

QString MillingMachine::CmdRepeatCallSubProgram(QString number, QString n_repeats) {
    QString cmd("");
    QTextStream stream(&cmd);
    stream << (program_line_number_++) << " CALL LBL " << number << " REP " << n_repeats << " /" << n_repeats << line_ending_;
    return cmd;
}

QString MillingMachine::CmdCallSubProgram(QString number) {
    QString cmd("");
    QTextStream stream(&cmd);
    stream << (program_line_number_++) << " CALL LBL " << number << " REP" << line_ending_;
    return cmd;
}

QString MillingMachine::CmdDefineStartSubProgram(QString number) {
    QString cmd("");
    QTextStream stream(&cmd);
    stream << (program_line_number_++) << " LBL " << number << line_ending_;
    return cmd;
}

QString MillingMachine::CmdDefineEndSubProgram() {
    QString cmd("");
    QTextStream stream(&cmd);
    stream << (program_line_number_++) << " LBL 0" << line_ending_;
    return cmd;
}

QString MillingMachine::CmdToolIncOneAxisMovement(QString axis, float pos, QString radius_comp, QString feed_rate, QString misc) {
    return CmdToolOneAxisMovement("I" + axis, pos, radius_comp, feed_rate, misc);
}

QString MillingMachine::CmdToolOneAxisMovement(QString axis, float pos, QString radius_comp, QString feed_rate, QString misc) {
    // Tool moves in straight line
    QString cmd("");
    QTextStream stream(&cmd);
    stream << (program_line_number_++);
    stream << " L " << axis << DoubleFormat(pos, 3) << " " << radius_comp << " " << feed_rate << " " << misc << line_ending_;
    return cmd;
}

QString MillingMachine::CmdToolIncTwoAxisMovement(QString axis1, float pos1, QString axis2, float pos2, QString radius_comp, QString feed_rate, QString misc) {
   return CmdToolTwoAxisMovement("I" + axis1, pos1, "I" + axis2, pos2, radius_comp, feed_rate, misc);
}

QString MillingMachine::CmdToolTwoAxisMovement(QString axis1, float pos1, QString axis2, float pos2, QString radius_comp, QString feed_rate, QString misc) {
    // Tool moves in straight line
    QString cmd("");
    QTextStream stream(&cmd);
    stream << (program_line_number_++);
    stream << " L " << axis1 << DoubleFormat(pos1, 3) << " " << axis2 << DoubleFormat(pos2, 3) << " " << radius_comp << " " << feed_rate << " " << misc << line_ending_;
    return cmd;
}

QString MillingMachine::CmdToolData(QString number, QString spindle_axis, QString spindle_speed) {
    QString cmd("");
    QTextStream stream(&cmd);
    stream << (program_line_number_++) << " TOOL CALL " << number << " " << spindle_axis << " S " << spindle_speed << line_ending_;
    return cmd;
}

QString MillingMachine::CmdDefineTool(QString number, QString length, QString radius) {
    QString cmd("");
    QTextStream stream(&cmd);
    stream << (program_line_number_++) << " TOOL DEF " << number << " " << length << " " << radius << line_ending_;
    return cmd;
}

QString MillingMachine::CmdBeginProgram(QString dossier) {
    QString cmd("");
    QTextStream stream(&cmd);
    stream << (program_line_number_++) << " BEGIN PGM " << dossier << " MM" << line_ending_;
    return cmd;
}

QString MillingMachine::CmdEndProgram(QString dossier) {
    QString cmd("");
    QTextStream stream(&cmd);
    stream << (program_line_number_++) << " END PGM " << dossier << " MM" << line_ending_;
    return cmd;
}

QString MillingMachine::CmdBlankForm(QString spindle_axis,
                                QString lower_x, QString lower_y, QString lower_z,
                                QString upper_x, QString upper_y, QString upper_z) {
    QString cmd("");
    QTextStream stream(&cmd);
    stream << (program_line_number_++) << " BLK FORM 0.1 " << spindle_axis << " " << lower_x << " " << lower_y << " " << lower_z << line_ending_;
    stream << (program_line_number_++) << " BLK FORM 0.2 " << upper_x << " " << upper_y << " " << upper_z << line_ending_;
    return cmd;
}

QString MillingMachine::DoubleFormat(const double &number, const int& digits) const {
    QString value(QString::number(number, 'f', digits));
    return ((number >= 0.0) ? "+" + value : value);
}
