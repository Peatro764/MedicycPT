#include <QtTest/QtTest>

class TestModulateur : public QObject {
    Q_OBJECT
private slots:
    void Constructors();
    void Comparison();
    void steps();
    void GetSectorAngularSizesNonDecreasingWeights();
    void GetSectorAngularSizes();
};
