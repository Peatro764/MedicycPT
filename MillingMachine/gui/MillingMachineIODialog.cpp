#include "MillingMachineIODialog.h"

#include <QGridLayout>
#include <unistd.h>
#include <QApplication>
#include <QDebug>

MillingMachineIODialog::MillingMachineIODialog(QWidget* parent, DataReader* data_reader) :
    QDialog(parent),
    data_reader_(data_reader)
{
    textLabel_ = new QLabel("Connecting...");
    textLabel_->setStyleSheet("font: 12pt;");
//    buttonBox_ = new QDialogButtonBox(QDialogButtonBox::Abort);
    buttonBox_ = new QDialogButtonBox(Qt::Horizontal);
    abortButton_ = new QPushButton("Abort");
    retryButton_ = new QPushButton("Retry");
    buttonBox_->addButton(retryButton_, QDialogButtonBox::ActionRole);
    buttonBox_->addButton(abortButton_, QDialogButtonBox::RejectRole);
    retryButton_->setVisible(false);

    progressBar_ = new QProgressBar;
    StartProgressBar();

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(textLabel_, 0, 0);
    mainLayout->addWidget(progressBar_, 1, 0);
    mainLayout->addWidget(buttonBox_, 2, 0);

    setLayout(mainLayout);
    setWindowTitle(tr("Fraiseuse"));

    QObject::connect(buttonBox_, SIGNAL(rejected()), this, SLOT(Abort()));
    QObject::connect(retryButton_, SIGNAL(clicked()), this, SLOT(Read()));
    QObject::connect(data_reader_, SIGNAL(ProgramRead(QString)), this, SLOT(DataReady(QString)));
    QObject::connect(data_reader_, SIGNAL(ReadError(QString)), this, SLOT(Error(QString)));
    QObject::connect(&timer_, SIGNAL(timeout()), this, SLOT(Read()));

    timer_.setSingleShot(true);
    timer_.start(500);
    QApplication::processEvents();
}

MillingMachineIODialog::~MillingMachineIODialog() {
    hide();
    accept();
}

void MillingMachineIODialog::Read() {
    qDebug() << "Read";
    StartProgressBar();
    retryButton_->setVisible(false);
    textLabel_->setText("Waiting for data...");
    QApplication::processEvents();
    usleep(200000);
    data_reader_->Read();
}

void MillingMachineIODialog::Abort() {
    qDebug() << "Abort";
    data_reader_->Abort();
    reject();
}

void MillingMachineIODialog::Error(QString error) {
    qDebug() << "Error";
    StopProgressBar();
    textLabel_->setText(error);
    QApplication::processEvents();
    retryButton_->setVisible(true);
}

void MillingMachineIODialog::DataReady(QString data) {
    qDebug() << "DataReady";
    StopProgressBar();
    data_ = data;
    qDebug() << "MillingMachineIODialog::DataReady " << data_;
    accept();
}

void MillingMachineIODialog::StopProgressBar() {
    qDebug() << "StopProgressBar";
    progressBar_->setVisible(false);
}

void MillingMachineIODialog::StartProgressBar() {
    progressBar_->setMinimum(0);
    progressBar_->setMaximum(0);
    progressBar_->setValue(0);
    progressBar_->setVisible(true);

}
