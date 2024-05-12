#ifndef SOCKETIOGUI_H
#define SOCKETIOGUI_H

#include <QWidget>

#include "SocketClient.h"

namespace Ui {
class SocketIOGui;
}

class SocketIOGui : public QWidget
{
    Q_OBJECT

public:
    explicit SocketIOGui(QWidget *parent = 0);
    ~SocketIOGui();

public slots:
    void WriteData();

private:
    Ui::SocketIOGui *ui_;
    SocketClient socket_client_;
};


#endif // FORM_H
