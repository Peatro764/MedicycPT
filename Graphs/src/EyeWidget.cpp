#include "EyeWidget.h"

#include <QDebug>
#include <QPainter>
#include <QPaintEvent>
#include <QGraphicsDropShadowEffect>

EyeWidget::EyeWidget(QWidget *parent) : QWidget(parent)
{
    background_ = QBrush(Qt::green);
    eye_pen_ = QPen(Qt::black);
    eye_pen_.setWidth(2);
}

void EyeWidget::PaintEye(const Eye &eye) {
    eye_ = eye;
    this->update();
}

void EyeWidget::EnableDropShadowEffect() {
    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();
    effect->setBlurRadius(5);
    setGraphicsEffect(effect);
//    setStyleSheet("border: 3px solid black");
}

void EyeWidget::paintEvent(QPaintEvent *event) {
    qDebug() << "paintEvent";
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

//    painter.fillRect(event->rect(), background_);
//    painter.translate(this->width()/2, this->height()/2);

    painter.setWindow(-15000, -15000, 30000, 30000);

//    painter.setWindow(-1000, -1000, 2000, 2000);
//    painter.setWindow(comp_poly.boundingRect());
    painter.scale(1.0, -1.0);
    eye_pen_.setWidth(50.0);

    QColor gold("#B9B9A5A54040");
    painter.setBrush(Qt::NoBrush);
    painter.setPen(eye_pen_);
    painter.drawLine(-15000, 0, 15000, 0);
    painter.drawLine(0, -15000, 0, 15000);

//    painter.setBrush(Qt::black);
    painter.drawPolygon(eye_.GetEyeBallPolygon());
    painter.drawPolygon(eye_.GetSclerePolygon());

    painter.end();
}
