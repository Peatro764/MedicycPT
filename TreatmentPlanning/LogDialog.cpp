#include "LogDialog.h"
#include "ui_LogDialog.h"

#include <QMessageBox>
#include <QFile>
#include <QTextStream>

LogDialog::LogDialog(QWidget *parent, QString path) :
    QDialog(parent),
    ui_(new Ui::LogDialog)
{
    ui_->setupUi(this);
    setWindowTitle(QString("Log"));
    SetupConnections();
    Fill(path);
}

LogDialog::~LogDialog()
{
    delete ui_;
}

void LogDialog::SetupConnections() {
    QObject::connect(ui_->closePushButton, SIGNAL(clicked()), this, SLOT(accept()));
}

void LogDialog::Fill(QString path) {
   QFile file(path);
   if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("MTD"), "Failed opening log file: " + file.errorString());
        reject();
   }

   QTextStream in(&file);
   while(!in.atEnd()) {
       ui_->textEdit->insertPlainText(in.readLine() + "\n");
   }
   file.close();
}
