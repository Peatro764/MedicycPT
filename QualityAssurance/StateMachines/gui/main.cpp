#include <QApplication>

#include <iostream>

#include "StateMachinesGui.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    StateMachinesGui gui(0);
    gui.show();
    return app.exec();
}


