#include <QtTest/QtTest>

#include "SOBP.h"

class TestSOBP : public QObject {
    Q_OBJECT
private:

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    SOBP GetStandardSOBP();

    void Getters();
    void Comparison();
};


