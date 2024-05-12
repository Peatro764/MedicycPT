#include <QtTest/QtTest>

#include "CompensateurMaker.h"

class TestCompensateurMaker : public QObject {
    Q_OBJECT
private:

private slots:
    void Discretize();
    void ScaleValueAxis();
    void SetMaxValue();
    void StepCurve();
    void GetCompensateur();
};


