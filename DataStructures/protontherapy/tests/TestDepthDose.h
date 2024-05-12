#include <QtTest/QtTest>

#include "DepthDose.h"

class TestDepthDose : public QObject {
    Q_OBJECT
private:

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void Constructor();
    void Comparison();
    void Addition();
    void Negation();
    void Subtraction();
    void Multiplication();
};


