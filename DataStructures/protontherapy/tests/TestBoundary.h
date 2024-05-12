#include <QtTest/QtTest>

#include "Boundary.h"

class TestBoundary : public QObject {
    Q_OBJECT
private:

private slots:
    void Comparison();
    void CropY();
    void CropX();
    void InvertY();
    void ScaleY();
    void MinY();
    void MaxY();
    void SetMinY();
    void SetMaxY();
    void StepFormat();
    void MirrorOverYAxis();
};


