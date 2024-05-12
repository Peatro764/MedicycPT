#include "MTD.h"

#include <QtGui>
#include <QApplication>
#include <QDebug>
#include <QSettings>
#include <QStandardPaths>

int main(int argc, char *argv[])
{   
    QSettings settings(QStandardPaths::locate(QStandardPaths::ConfigLocation,
                                              QString("MTD.ini"),
                                              QStandardPaths::LocateFile), QSettings::IniFormat);
    if (QFile(settings.fileName()).exists()) {
        qDebug() << "MTD.ini found";
    } else {
        qWarning() << "MTD.ini not found";
        return -1;
    }

    QApplication app(argc, argv);
    QFile file(":/dark.qss");
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "main: cannot open dark.qss";
    }
    QTextStream stream(&file);
    app.setStyleSheet(stream.readAll());
    MTD gui(0);
    gui.setStyleSheet(stream.readAll());
    gui.show();
    return app.exec();
}

