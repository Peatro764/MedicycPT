#include "RecordingWidget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QDebug>

RecordingWidget::RecordingWidget(QWidget *parent)
    : QWidget(parent) {
    setAttribute(Qt::WA_StaticContents);
    setStyleSheet(QString("background: transparent;"));
    timer_.setInterval(time_);
    timer_.setSingleShot(false);
    QObject::connect(&timer_, &QTimer::timeout, this, &RecordingWidget::Flip);
}

void RecordingWidget::Off() {
    timer_.stop();
    paint_ = false;
    update();
}

void RecordingWidget::On() {
    timer_.start();
}

void RecordingWidget::SetTime(int time) {
    time_ = time;
    timer_.setInterval(time);
}

void RecordingWidget::Flip() {
    paint_ = !paint_;
    update();
}

void RecordingWidget::paintEvent(QPaintEvent *event) {
    if (paint_) {
        QPainter painter(this);
        QPen pen(Qt::red);
        QBrush brush(Qt::red);
        painter.setPen(pen);
        painter.setBrush(brush);
        painter.drawEllipse(QPoint(20, 20), 20, 20);
    }
}
