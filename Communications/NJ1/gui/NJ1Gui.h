#ifndef NJ1GUI_H
#define NJ1GUI_H

#include <QWidget>
#include <memory>
#include <QSettings>

#include "NJ1.h"

namespace Ui {
class NJ1Gui;
}

class NJ1Gui : public QWidget
{
    Q_OBJECT

public:
    explicit NJ1Gui(QWidget *parent = 0);
    ~NJ1Gui();

public slots:

private slots:
    void Connect();
    void Disconnect();
    void UploadSeance();

private:
    Ui::NJ1Gui *ui_;
    std::unique_ptr<QSettings> settings_;
    nj1::NJ1 nj1_;
};


#endif
