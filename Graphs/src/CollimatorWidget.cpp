#include "CollimatorWidget.h"

#include <QDebug>
#include <QPainter>
#include <QPaintEvent>
#include <QGraphicsDropShadowEffect>

CollimatorWidget::CollimatorWidget(QWidget *parent) : QWidget(parent)
{
    QColor bg_color = QColor("#31363b");
    background_ = QBrush(bg_color);
    collimator_pen_ = QPen(Qt::black);
    collimator_pen_.setWidth(2);
}

void CollimatorWidget::PaintCollimator(const Collimator &collimator) {
    collimator_ = collimator;
    this->update();
}

void CollimatorWidget::EnableDropShadowEffect() {
    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();
    effect->setBlurRadius(5);
    setGraphicsEffect(effect);
//    setStyleSheet("border: 3px solid black");
}

void CollimatorWidget::paintEvent(QPaintEvent *event) {
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(event->rect(), background_);
    painter.translate(this->width()/2, this->height()/2);
    painter.scale(1.0, -1.0);

    if (!collimator_.InternalEdges().empty()) {

        QPolygon coll_poly = collimator_.InternalEdgesPylogonFormat();

        painter.setWindow(-d_coll_um_ / 2, -d_coll_um_ / 2, d_coll_um_, d_coll_um_);
        collimator_pen_.setWidth(1.0);

        QColor gold("#B9B9A5A54040");
        painter.setBrush(gold);
        painter.setPen(collimator_pen_);
        const int circle_radius(d_coll_um_ / 2);
        painter.drawChord(-circle_radius, -circle_radius,
                        2 * circle_radius, 2 * circle_radius,
                        0, 5760);
        painter.setBrush(Qt::black);
        painter.drawPolygon(coll_poly);
    }

    painter.end();
}
