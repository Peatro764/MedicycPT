#include <QApplication>

#include <iostream>
#include <QSettings>
#include <QStandardPaths>
#include "MQA.h"

int main(int argc, char *argv[])
{
    QSettings settings(QStandardPaths::locate(QStandardPaths::ConfigLocation,
                                              QString("MQA.ini"),
                                              QStandardPaths::LocateFile), QSettings::IniFormat);
    if (QFile(settings.fileName()).exists()) {
        qDebug() << "MQA.ini found";
    } else {
        qWarning() << "MQA.ini not found";
        return -1;
    }


    QApplication app(argc, argv);
    QFile file(":/dark.qss");
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    app.setStyleSheet(stream.readAll());
    MQA gui(0);    
    gui.show();
    return app.exec();
}


