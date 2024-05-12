#include <QApplication>
#include <QFile>
#include <iostream>
#include <QSettings>
#include <QStandardPaths>

#include "RadiationMonitor.h"

int main(int argc, char *argv[])
{
    QSettings settings(QStandardPaths::locate(QStandardPaths::ConfigLocation,
                                              QString("MRM.ini"),
                                              QStandardPaths::LocateFile), QSettings::IniFormat);
    if (QFile(settings.fileName()).exists()) {
        qDebug() << "MRM.ini found";
    } else {
        qWarning() << "MRM.ini not found";
        return -1;
    }

    QApplication app(argc, argv);
    QFile file(":/dark.qss");
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    app.setStyleSheet(stream.readAll());
    RadiationMonitor gui(0);
    gui.show();
    return app.exec();
}


