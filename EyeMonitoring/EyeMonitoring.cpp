#include "EyeMonitoring.h"
#include "ui_EyeMonitoring.h"

#include <QDateTime>
#include <QColorDialog>
#include <QtGlobal>
#include <QTimer>
#include <QProcess>
#include <QStandardPaths>

#include "NumericInputDialog.h"
#include "TimedState.h"
#include "Patient.h"
#include "XRayRecord.h"

using namespace ::utils;

EyeMonitoring::EyeMonitoring(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EyeMonitoring),
    settings_(std::unique_ptr<QSettings>(new QSettings(QStandardPaths::locate(QStandardPaths::ConfigLocation, QString("MEM.ini"), QStandardPaths::LocateFile),
                                                          QSettings::IniFormat))),
    mrm_thread_(new MRMThread(parent)),
    video_worker_(new VideoWorker),
    db_worker_(new DBWorker) {

    ui->setupUi(this);
    setWindowTitle(QString("Medicyc Eye Monitoring (MEM) v1.0.0"));
    InitPenStyle();

    qRegisterMetaType<Patient>("Patient");

    ui->graphicsView_videofeed->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_videofeed->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    qDebug() << "EyeMonitoring Gui thread " << QThread::currentThreadId();

    // MRM Thread
    mrm_thread_->start();
    QObject::connect(mrm_thread_, &MRMThread::SIGNAL_XRayDetected, this, &EyeMonitoring::XRayDetected);
    QObject::connect(mrm_thread_, &MRMThread::SIGNAL_Error, this, &EyeMonitoring::DisplayError);

    // Video Thread
    video_worker_->moveToThread(&video_thread_);
    QObject::connect(&video_thread_, &QThread::finished, &video_thread_, &QObject::deleteLater);
    QObject::connect(video_worker_, &VideoWorker::SIGNAL_NoVideoFeed, this, &EyeMonitoring::DisplayNoVideoFeedMessage);
    QObject::connect(video_worker_, &VideoWorker::SIGNAL_VideoFeedRead, this, &EyeMonitoring::UpdateVideoImage);
    QObject::connect(video_worker_, &VideoWorker::SIGNAL_Error, this, &EyeMonitoring::DisplayError);
    QTimer::singleShot(1000, video_worker_, &VideoWorker::Start);
    video_thread_.start();

    // DB Thread
    db_worker_->moveToThread(&db_thread_);
    QObject::connect(&db_thread_, &QThread::finished, &db_thread_, &QObject::deleteLater);
    QObject::connect(this, &EyeMonitoring::SIGNAL_XRayDetected, db_worker_, &DBWorker::SaveXRayRecord);
    QObject::connect(this, &EyeMonitoring::SIGNAL_LoadPatient, db_worker_, &DBWorker::GetPatient);
    QObject::connect(db_worker_, &DBWorker::SIGNAL_Patient, this, [&](Patient patient) { ui->label_patient->setText(patient.GetLastName().toUpper() + " " + patient.GetFirstName()); });
    QObject::connect(db_worker_, &DBWorker::SIGNAL_Error, this, &EyeMonitoring::DisplayError);
    QTimer::singleShot(1000, db_worker_, &DBWorker::Setup);
    db_thread_.start();

    // RECORDINGS
    recording_abort_timer_.setInterval(60000);
    QObject::connect(mrm_thread_, &MRMThread::SIGNAL_IrradiationStarted, &recording_abort_timer_, qOverload<>(&QTimer::start));
    QObject::connect(&recording_abort_timer_, &QTimer::timeout, mrm_thread_, &MRMThread::SIGNAL_IrradiationStopped);
    QObject::connect(mrm_thread_, &MRMThread::SIGNAL_IrradiationStarted, ui->widget_recording, &RecordingWidget::On);
    QObject::connect(mrm_thread_, &MRMThread::SIGNAL_IrradiationStopped, ui->widget_recording, &RecordingWidget::Off);
    QObject::connect(mrm_thread_, &MRMThread::SIGNAL_IrradiationStarted, this, [&]() { emit SIGNAL_StartRecording(GetDossier()); });
    QObject::connect(this, &EyeMonitoring::SIGNAL_StartRecording, video_worker_, &VideoWorker::StartRecording);
    QObject::connect(mrm_thread_, &MRMThread::SIGNAL_IrradiationStopped, video_worker_, &VideoWorker::StopRecording);

    ui->graphicsView_videofeed->setScene(new QGraphicsScene(this));
    ui->graphicsView_videofeed->scene()->addItem(&pixmap);

    QObject::connect(ui->pushButton_clear, &QPushButton::clicked, this, &EyeMonitoring::ClearDrawArea);
    QObject::connect(ui->pushButton_exit, &QPushButton::clicked, this, &EyeMonitoring::Exit);
    QObject::connect(ui->pushButton_color_dialog, &QPushButton::clicked, this, &EyeMonitoring::ShowColorDialog);

//    QObject::connect(ui->pushButton_dash, &QPushButton::clicked, ui->widget_drawing_area, &DrawingArea::SetPenDashed);
//    QObject::connect(ui->pushButton_solid, &QPushButton::clicked, ui->widget_drawing_area, &DrawingArea::SetPenSolid);

//    QObject::connect(ui->spinBox_penwidth, qOverload<int>(&QSpinBox::valueChanged), ui->widget_drawing_area, &DrawingArea::SetPenWidth);
//    QObject::connect(ui->spinBox_dash_separation, qOverload<int>(&QSpinBox::valueChanged), ui->widget_drawing_area, &DrawingArea::SetDashSeparation);

    QObject::connect(ui->pushButton_show_numeric_keyboard, &QPushButton::clicked, this, &EyeMonitoring::ShowNumericKeyboard);
}

EyeMonitoring::~EyeMonitoring() {
    qDebug() << "~EyeMonitoring Entry";
    mrm_thread_->terminate();
    video_thread_.terminate();
    db_thread_.terminate();
    qDebug() << "~EyeMonitoring Exit";
}

void EyeMonitoring::ShowNumericKeyboard() {
    NumericInputDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        ui->label_patient->clear();
        ui->label_xrays->setText("0");
        ui->pushButton_show_numeric_keyboard->setText(QString::number(dialog.Dossier()));
        emit SIGNAL_LoadPatient(dialog.Dossier());
    }
}

int EyeMonitoring::GetDossier() {
    return ui->pushButton_show_numeric_keyboard->text().toInt();
}

void EyeMonitoring::XRayDetected(double peak_rate, double integrated_charge) {
    const int dossier = GetDossier();
    if (!dossier) {
        qDebug() << "EyeMonitoring::XRayDetected No patient loaded, x-ray ignored";
        return;
    }
    ui->label_xrays->setText(QString::number((ui->label_xrays->text().toInt() + 1)));
    emit SIGNAL_XRayDetected(dossier, peak_rate, integrated_charge);
}

//void EyeMonitoring::GetAvailableCameras() {
//    QProcess process;
//    process.start("v4l2-ctl --list-devices");
//    process.waitForFinished(-1); // will wait forever until finished
//    QString reply = process.readAllStandardOutput();
//    QStringList list = reply.split("\n");
//    qDebug() << "STDOUT " << list;
//}

void EyeMonitoring::InitPenStyle() {
    ui->widget_drawing_area->SetPenColor(Qt::magenta);
    ui->widget_drawing_area->SetPenWidth(7);//ui->spinBox_penwidth->value());
    ui->widget_drawing_area->SetPenDashed();
    ui->widget_drawing_area->SetDashSeparation(2);//ui->spinBox_dash_separation->value());
}

void EyeMonitoring::ClearDrawArea() {
    ui->widget_drawing_area->ClearArea();
}

void EyeMonitoring::ShowColorDialog() {
    QString color;
    QColorDialog dialog(this);
    dialog.setCurrentColor (ui->widget_drawing_area->PenColor());

    if (dialog.exec() == QColorDialog::Accepted) {
        ui->widget_drawing_area->SetPenColor(dialog.currentColor());
    }
}

void EyeMonitoring::DisplayNoVideoFeedMessage(int channel) {
    QImage blank_image(ui->graphicsView_videofeed->size(), QImage::Format_ARGB32);
    blank_image.fill(qRgb(0, 0, 0));
    QPainter p(&blank_image);
    p.setPen(QPen(Qt::white));
    p.setFont(QFont("Ubuntu", 20, QFont::Bold));
    p.drawText(blank_image.rect(), Qt::AlignCenter, QString("En attente de vidéo sur canal %1...").arg(channel));
    pixmap.setPixmap(QPixmap::fromImage(blank_image));
    ui->graphicsView_videofeed->fitInView(&pixmap, Qt::KeepAspectRatioByExpanding);
}

void EyeMonitoring::UpdateVideoImage(const QImage &image) {
//    pixmap.setPixmap(QPixmap::fromImage(image));
//    qDebug() << "SIZE " << tmp.size();
    pixmap.setPixmap(QPixmap::fromImage(image));//.copy(0, 44, 720, 531)); //.copy(20, 32, 700, 544));
    ui->graphicsView_videofeed->fitInView(&pixmap, Qt::KeepAspectRatio);
}

//void EyeMonitoring::closeEvent(QCloseEvent *event) {
//    if (video.isOpened()) {
//        video.release();
//    }
//    event->accept();
//}

void EyeMonitoring::DisplayError(QString message) {
    DisplayModelessMessageBox(message, true, 4, QMessageBox::Critical);
}

void EyeMonitoring::Exit() {
    exit(1);
}

void EyeMonitoring::DisplayModelessMessageBox(QString msg, bool auto_close, int sec, QMessageBox::Icon icon) {
    QMessageBox* box = new QMessageBox(this);
    box->setText(msg);
    box->setIcon(icon);
    box->setModal(false);
    box->setAttribute(Qt::WA_DeleteOnClose);
    if (auto_close) QTimer::singleShot(sec * 1000, box, SLOT(close()));
    box->show();
}
