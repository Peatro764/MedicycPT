#include <QApplication>

#include <iostream>

#include "TestGraphs.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestGraphs gui(0);
    gui.show();
    return app.exec();
}


