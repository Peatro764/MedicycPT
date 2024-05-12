#ifndef EYEWIDGET_H
#define EYEWIDGET_H

#include <QBrush>
#include <QPen>
#include <QWidget>

#include "Eye.h"

class EyeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EyeWidget(QWidget *parent = 0);

signals:

public slots:
    void PaintEye(const Eye& eye);
    void EnableDropShadowEffect();

protected:
    void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;

private:
    QBrush background_;
    QPen eye_pen_;
    Eye eye_;
};

#endif // EYEWIDGET_H
