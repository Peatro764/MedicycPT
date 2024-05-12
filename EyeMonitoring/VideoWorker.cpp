#include "VideoWorker.h"

#include <QCoreApplication>
#include <QStandardPaths>
#include <QDebug>
#include <QThread>
#include <QDateTime>

#include "TimedState.h"

using namespace ::utils;

VideoWorker::VideoWorker(QObject* parent)
    : QObject(parent),
      settings_(std::unique_ptr<QSettings>(new QSettings(QStandardPaths::locate(QStandardPaths::ConfigLocation, QString("MEM.ini"), QStandardPaths::LocateFile),
                                                            QSettings::IniFormat))) {
}

VideoWorker::~VideoWorker() {
    if (video.isOpened()) {
        video.release();
    }
    qDebug() << "~VideoWorker";
}

void VideoWorker::Start() {
    qDebug() << "VideoWorker::Start " << QThread::currentThreadId();
    sm_ = new QStateMachine(this);

    // States
    QState *sSuperState = new QState();
    TimedState *sWaitingForFeed = new TimedState(sSuperState, 2000, "Reentry");
    QState *sReadingFeed = new QState(sSuperState);
    TimedState *sDelay = new TimedState(sSuperState, settings_->value("video/delay", "0").toInt(), "Delay", sReadingFeed);

    // SuperState
    sSuperState->setInitialState(sWaitingForFeed);

    // WaitingForFeed
    QObject::connect(sWaitingForFeed, &QState::entered, this, [&](){ emit SIGNAL_NoVideoFeed(camera_channel_); });
    QObject::connect(sWaitingForFeed, &QState::entered, this, &VideoWorker::OpenVideoFeed);
    sWaitingForFeed->addTransition(this, &VideoWorker::SIGNAL_VideoFeedOpen, sReadingFeed);

    // ReadingFeed
    QObject::connect(sReadingFeed, &QState::entered, this, [&]() { qApp->processEvents(); });
    QObject::connect(sReadingFeed, &QState::entered, this, &VideoWorker::ReadVideoFeed);
    sReadingFeed->addTransition(this, &VideoWorker::SIGNAL_VideoFeedRead, sDelay);
    sReadingFeed->addTransition(this, &VideoWorker::SIGNAL_VideoFeedClosed, sWaitingForFeed);

    // Delay
    (void)sDelay;

//    PrintStateChanges(sSuperState, "SuperState");
//    PrintStateChanges(sWaitingForFeed, "WaitingForFeed");
//    PrintStateChanges(sReadingFeed, "ReadingFeed");

    sm_->addState(sSuperState);
    sm_->setInitialState(sSuperState);
    sm_->start();
}

void VideoWorker::OpenVideoFeed() {
    camera_channel_ = settings_->value("video/camera_channel", "0").toInt();
    if (video.open(camera_channel_)) {
        emit SIGNAL_VideoFeedOpen();
    } else {
        emit SIGNAL_VideoFeedClosed();
    }
}

void VideoWorker::CloseVideoFeed() {
    video.release();
    emit SIGNAL_VideoFeedClosed();
}

void VideoWorker::ReadVideoFeed() {
    if(video.isOpened()) {
        Mat frame;
        video >> frame;
        if(!frame.empty()) {
            QImage qimg(frame.data,
                        frame.cols,
                        frame.rows,
                        frame.step,
                        QImage::Format_RGB888);
            emit SIGNAL_VideoFeedRead(qimg.rgbSwapped());
        }
        if (record_) {
            recorder_.write(frame);
        }
    } else {
        emit SIGNAL_VideoFeedClosed();
    }
}

void VideoWorker::StartRecording(int dossier) {
    qDebug() << "StartRecording " << QThread::currentThreadId();
    recorder_.release();
    QString recordings_dir = settings_->value("video/recordings_dir", "").toString();
    QString filename = recordings_dir + QString::number(dossier) + "_" + QDateTime::currentDateTime().toString("yyMMdd-HHmmss") + ".avi";
    qDebug() << "Recording filename " << filename;
    recorder_ = cv::VideoWriter(filename.toStdString(), cv::VideoWriter::fourcc('M','J','P','G'),
                             10,
                             Size(video.get(cv::CAP_PROP_FRAME_WIDTH),
                                  video.get(cv::CAP_PROP_FRAME_HEIGHT)));
    record_ = true;
}

void VideoWorker::StopRecording() {
    qDebug() << "StopRecording " << QThread::currentThreadId();
    record_ = false;
    recorder_.release();
}

void VideoWorker::PrintStateChanges(QState *state, QString name) {
    QObject::connect(state, &QState::entered, this, [&, name]() { qDebug() << "MEM " << QDateTime::currentDateTime() << " ->" << name << " " << QThread::currentThreadId(); });
    QObject::connect(state, &QState::exited, this, [&, name]() { qDebug() << "MEM " << QDateTime::currentDateTime() << " <-" << name << " " << QThread::currentThreadId(); });
}


