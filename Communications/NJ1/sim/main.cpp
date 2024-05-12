#include <QApplication>

#include <iostream>

#include "NJ1Sim.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    nj1::NJ1Sim gui(0);
    gui.show();
    return app.exec();
}


