#include <QApplication>
#include <qapplication.h>
#include <iostream>
#include <QFile>
#include <QTextStream>
#include <QtGlobal>
#include <QSettings>
#include <QStandardPaths>

#include "MTP.h"


void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    const std::string red("\033[0;31m");
    const std::string reset("\033[0m");

    QByteArray localMsg = msg.toLocal8Bit();

    switch (type) {
    case QtDebugMsg:
        std::cout << red << "Debug: " << localMsg.constData() << context.file << context.line << context.function << std::endl;
        break;
    case QtInfoMsg:
        std::cout << red << "Info: " << localMsg.constData() << context.file << context.line << context.function << std::endl;
        break;
    case QtWarningMsg:
        std::cout << "Warning: " << localMsg.constData() << std::endl;
        break;
    case QtCriticalMsg:
        std::cout << "Critical: " << localMsg.constData() << std::endl;
        break;
    case QtFatalMsg:
        std::cout << "Fatal: " << localMsg.constData() << std::endl;
        break;
    }
}

int main(int argc, char *argv[])
{
    QSettings settings(QStandardPaths::locate(QStandardPaths::ConfigLocation,
                                              QString("MTP.ini"),
                                              QStandardPaths::LocateFile), QSettings::IniFormat);
    if (QFile(settings.fileName()).exists()) {
        qDebug() << "MQA.ini found";
    } else {
        qWarning() << "MQA.ini not found";
        return -1;
    }

//    qInstallMessageHandler(myMessageOutput);
    QApplication app(argc, argv);

    QFile file(":/dark.qss");
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    app.setStyleSheet(stream.readAll());

    MTP mtp(0);
    mtp.show();
    return app.exec();
}


