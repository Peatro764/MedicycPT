#ifndef VideoWorker_H
#define VideoWorker_H

#include <QStateMachine>
#include <QTimer>
#include "opencv2/opencv.hpp"
#include <memory>
#include <QSettings>
#include <QImage>

using namespace cv;

class VideoWorker : public QObject {
  Q_OBJECT

   public:
    VideoWorker(QObject* parent = 0);
    ~VideoWorker();

signals:
    void SIGNAL_NoVideoFeed(int channel);
    void SIGNAL_VideoFeedOpen();
    void SIGNAL_VideoFeedClosed();
    void SIGNAL_VideoFeedRead(const QImage &image);
    void SIGNAL_Error(QString message);

public slots:
    void Start();
    void StartRecording(int dossier);
    void StopRecording();

private slots:
    void OpenVideoFeed();
    void CloseVideoFeed();
    void ReadVideoFeed();

private:
    void PrintStateChanges(QState *state, QString name);
    std::unique_ptr<QSettings> settings_;
    cv::VideoCapture video;
    int camera_channel_ = 0;
    cv::VideoWriter recorder_;
    bool record_ = false;
    QStateMachine *sm_;
};


#endif // VideoWorker_H
