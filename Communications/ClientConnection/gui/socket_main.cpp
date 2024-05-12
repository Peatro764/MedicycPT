#include <QApplication>

#include <iostream>

#include "SocketIOGui.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    SocketIOGui gui(0);
    gui.show();
    return app.exec();
}


