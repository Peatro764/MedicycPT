#include "DrawingArea.h"

#include <QMouseEvent>
#include <QPainter>
#include <QDebug>

DrawingArea::DrawingArea(QWidget *parent)
    : QWidget(parent) {
    setAttribute(Qt::WA_StaticContents);
    setStyleSheet(QString("background: transparent;"));
    pen_ =  QPen(pen_color_, pen_width_, Qt::SolidLine, Qt::RoundCap,
         Qt::RoundJoin);
}

void DrawingArea::SetPenColor(const QColor &newColor) {
    pen_.setColor(newColor);
}

void DrawingArea::SetPenWidth(int newWidth) {
    pen_.setWidth(newWidth);
}

void DrawingArea::SetPenSolid() {
    pen_.setStyle(Qt::SolidLine);
}

void DrawingArea::SetPenDashed() {
    pen_.setStyle(Qt::DashLine);
}

void DrawingArea::SetDashSeparation(int sep) {
    QVector<qreal> dashes;
    const int dash_length(3);
    dashes << dash_length << sep*dash_length << dash_length << sep*dash_length;
    pen_.setDashPattern(dashes);
}

void DrawingArea::ClearArea() {
    image_.fill(qRgba(0, 0, 0, 0));
    modified_ = true;
    update();
}

void DrawingArea::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        path_ = QPainterPath(event->pos());
        scribbling_ = true;
    }
}

void DrawingArea::mouseMoveEvent(QMouseEvent *event) {
    if ((event->buttons() & Qt::LeftButton) && scribbling_)
        DrawLineTo(event->pos());
}

void DrawingArea::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && scribbling_) {
        DrawLineTo(event->pos());
        scribbling_ = false;
    }
}

void DrawingArea::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, image_, dirtyRect);
}

void DrawingArea::resizeEvent(QResizeEvent *event) {
    if (width() > image_.width() || height() > image_.height()) {
        int newWidth = qMax(width() + 128, image_.width());
        int newHeight = qMax(height() + 128, image_.height());
        resizeImage(&image_, QSize(newWidth, newHeight));
        update();
    }
    QWidget::resizeEvent(event);
}

void DrawingArea::resizeImage(QImage *image, const QSize &newSize) {
    if (image->size() == newSize)
        return;
    QImage newImage(newSize, QImage::Format_ARGB32);
    newImage.fill(qRgba(0, 0, 0, 0));
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), *image);
    *image = newImage;
}

void DrawingArea::DrawLineTo(const QPoint &endPoint) {
    QPainter painter(&image_);
    painter.setPen(pen_);
    path_.lineTo(endPoint);
    painter.drawPath(path_);
    modified_ = true;

    int rad = (pen_width_ / 2) + 2;
    update(QRect(last_point_, endPoint).normalized()
                                     .adjusted(-rad, -rad, +rad, +rad));
    last_point_ = endPoint;
}

