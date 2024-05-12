#include <QtTest/QtTest>

#include "DepthDoseCurve.h"

class TestDepthDoseCurve : public QObject {
    Q_OBJECT
private:

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void Constructor();
    void Comparison();
    void CapAtZero();
    void Dose();
    void Addition();
    void Negation();
    void Subtraction();
    void Multiplication();
    void Division();
    void Max();
    void LeftMostMax();
    void RightMostMax();
    void UpperRange();
    void LowerRange();
    void Degrade();
    void MaxParcours();
    void CorrespondingDepthFromRight();
    void Penumbra();
    void Flatness();
};


