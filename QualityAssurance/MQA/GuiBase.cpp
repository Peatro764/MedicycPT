#include "GuiBase.h"

#include "ui_MQA.h"
#include "MQA.h"

#include <QMessageBox>
#include <QDebug>
#include <QStandardPaths>

GuiBase::GuiBase(MQA *parent) :
    parent_(parent), settings_(QStandardPaths::locate(QStandardPaths::ConfigLocation, QString("MQA.ini"), QStandardPaths::LocateFile),
                               QSettings::IniFormat)
{
}

GuiBase::~GuiBase() {}

void GuiBase::DisplayModelessMessageBox(QString msg, bool auto_close, int sec, QMessageBox::Icon icon) {
    qDebug() << "GuiBase::DisplayModelessMessageBox " << msg;
    QMessageBox* box = new QMessageBox(parent_);
    box->setText(msg);
    box->setIcon(icon);
    box->setModal(false);
    box->setAttribute(Qt::WA_DeleteOnClose);
    if (auto_close) QTimer::singleShot(sec * 1000, box, SLOT(close()));
    box->show();
}

void GuiBase::DisplayInfo(QString msg) {
    DisplayModelessMessageBox(msg, true, 5, QMessageBox::Information);
}

void GuiBase::DisplayWarning(QString msg) {
    DisplayModelessMessageBox(msg, true, 10, QMessageBox::Warning);
}

void GuiBase::DisplayError(QString msg) {
    DisplayModelessMessageBox(msg, true, 60, QMessageBox::Critical);
}

void GuiBase::DisplayCritical(QString msg) {
    DisplayModelessMessageBox(msg, false, 600, QMessageBox::Critical);
}

QString GuiBase::ButtonStyleSheet(QString image) {
    QString str ("QPushButton {"
                 "background-image: url(:/images/" + image + ");"
    "background-repeat: no-repeat;"
    "background-position: center;"
    "border-style: raised; "
    "border-width: 0px;"
    "border-radius: 25px;"
    "}"
    "QPushButton:pressed {"
//    "border-style: 4px;"
    "border-width: 0px;"
    "border-color: white;"
    "}");
    return str;
}

QVariant GuiBase::GetSetting(QString variable) const {
    return settings_.value(variable);
}


