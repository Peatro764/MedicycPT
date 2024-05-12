#include <QApplication>

#include <iostream>

#include "DataAcquisitionGui.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    DataAcquisitionGui gui(0);
    gui.show();
    return app.exec();
}


