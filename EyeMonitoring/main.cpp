#include "EyeMonitoring.h"

#include <QtGui>
#include <QApplication>
#include <QDebug>
#include <QSettings>
#include <QStandardPaths>

int main(int argc, char *argv[])
{   
    QSettings settings(QStandardPaths::locate(QStandardPaths::ConfigLocation,
                                              QString("MEM.ini"),
                                              QStandardPaths::LocateFile), QSettings::IniFormat);
    if (QFile(settings.fileName()).exists()) {
        qDebug() << "MEM.ini found";
    } else {
        qWarning() << "MEM.ini not found";
        return -1;
    }

    QApplication app(argc, argv);
    QFile file(":/dark.qss");
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "main: cannot open dark.qss";
    }
    QTextStream stream(&file);
    app.setStyleSheet(stream.readAll());
    EyeMonitoring gui(0);
    gui.setStyleSheet(stream.readAll());
    gui.setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    gui.show();
    return app.exec();
}

