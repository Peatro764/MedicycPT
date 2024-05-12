#ifndef RecordingWidget_H
#define RecordingWidget_H

#include <QColor>
#include <QPen>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QTimer>

class RecordingWidget : public QWidget
{
    Q_OBJECT

public:
    RecordingWidget(QWidget *parent);

public slots:
    void SetTime(int time);
    void On();
    void Off();

private slots:
    void Flip();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int time_ = 800;
    bool paint_ = false;
    QTimer timer_;
};

#endif
