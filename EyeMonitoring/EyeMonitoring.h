#ifndef EYEMONITORING_H
#define EYEMONITORING_H

#include <QSettings>
#include <QStateMachine>
#include <QMainWindow>
#include <QDebug>
#include <QGraphicsScene>
#include <QPainterPath>
#include <QGraphicsPixmapItem>
#include <QImage>
#include <QPixmap>
#include <QCloseEvent>
#include <QMessageBox>
#include <memory>

#include "DrawingArea.h"
#include "MRMThread.h"
#include "VideoWorker.h"
#include "DBWorker.h"

namespace Ui {
class EyeMonitoring;
}

class EyeMonitoring : public QMainWindow
{
    Q_OBJECT

public:
    explicit EyeMonitoring(QWidget *parent = 0);
    ~EyeMonitoring();

protected:

private slots:
    void UpdateVideoImage(const QImage &image);
    void ShowNumericKeyboard();
    void InitPenStyle();
    void ShowColorDialog();
    void DisplayNoVideoFeedMessage(int channel);
    void ClearDrawArea();
    void XRayDetected(double peak_rate, double integrated_charge);
    void DisplayError(QString message);
    void Exit();

signals:
    void SIGNAL_StartRecording(int dossier);
    void SIGNAL_XRayDetected(int dossier, double peak_value, double integrated_charge);
    void SIGNAL_LoadPatient(int dossier);

private:
    int GetDossier();
    void DisplayModelessMessageBox(QString msg, bool auto_close, int sec, QMessageBox::Icon icon);
    Ui::EyeMonitoring *ui;

    QGraphicsPixmapItem pixmap;
    QTimer recording_abort_timer_;
    std::unique_ptr<QSettings> settings_;
    MRMThread *mrm_thread_;
    VideoWorker *video_worker_;
    QThread video_thread_;
    DBWorker *db_worker_;
    QThread db_thread_;
};

#endif
