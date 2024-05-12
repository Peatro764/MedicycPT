#include <QApplication>

#include <iostream>

#include "NJ1Gui.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    NJ1Gui gui(0);
    gui.show();
    return app.exec();
}


