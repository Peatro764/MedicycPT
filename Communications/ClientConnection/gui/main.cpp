#include <QApplication>

#include <iostream>

#include "ClientConnectionGui.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ClientConnectionGui gui(0);
    gui.show();
    return app.exec();
}


