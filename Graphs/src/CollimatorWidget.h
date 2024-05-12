#ifndef COLLIMATORWIDGET_H
#define COLLIMATORWIDGET_H

#include <QBrush>
#include <QPen>
#include <QWidget>

#include "Collimator.h"

class CollimatorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CollimatorWidget(QWidget *parent = 0);

signals:

public slots:
    void PaintCollimator(const Collimator& collimator);
    void EnableDropShadowEffect();

protected:
    void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;

private:
    QBrush background_;
    QPen collimator_pen_;
    Collimator collimator_;

    const int d_coll_um_ = 34000;
};

#endif // COLLIMATORWIDGET_H
