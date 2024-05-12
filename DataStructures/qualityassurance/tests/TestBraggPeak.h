#include <QtTest/QtTest>

#include "BraggPeak.h"

class TestBraggPeak : public QObject {
    Q_OBJECT
private:

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    BraggPeak GetStandardBraggPeak();
    void Getters();
    void Comparison();
};


