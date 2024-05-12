#include <QApplication>
#include <QSettings>
#include <QStandardPaths>

#include "MillingMachineGui.h"

int main(int argc, char *argv[])
{
    QSettings settings(QStandardPaths::locate(QStandardPaths::ConfigLocation,
                                              QString("MMM.ini"),
                                              QStandardPaths::LocateFile), QSettings::IniFormat);
    if (QFile(settings.fileName()).exists()) {
        qDebug() << "MMM.ini found";
    } else {
        qWarning() << "MMM.ini not found";
        return -1;
    }

    QApplication app(argc, argv);

    QFile file(":/dark.qss");
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    app.setStyleSheet(stream.readAll());

    MillingMachineGui gui(0);
    gui.show();
    return app.exec();
}


