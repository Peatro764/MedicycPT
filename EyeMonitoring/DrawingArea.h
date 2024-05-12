#ifndef DRAWINGAREA_H
#define DRAWINGAREA_H

#include <QColor>
#include <QPen>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QPainterPath>

class DrawingArea : public QWidget
{
    Q_OBJECT

public:
    DrawingArea(QWidget *parent = nullptr);
    bool IsModified() const { return modified_; }
    QColor PenColor() const { return pen_color_; }
    int PenWidth() const { return pen_width_; }

public slots:
    void SetPenColor(const QColor &newColor);
    void SetPenWidth(int newWidth);
    void SetPenSolid();
    void SetPenDashed();
    void SetDashSeparation(int sep);
    void ClearArea();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void DrawLineTo(const QPoint &endPoint);
    void resizeImage(QImage *image, const QSize &newSize);

    QPainterPath path_;
    bool modified_ = false;
    bool scribbling_ = false;
    int pen_width_ = 2;
    QColor pen_color_ = Qt::black;
    QImage image_;
    QPoint last_point_;
    QPen pen_;
};

#endif
