#include <QtTest/QtTest>

#include "BeamProfile.h"

class TestBeamProfile : public QObject {
    Q_OBJECT
private:

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void Comparisons();
    void Centre();
    void Width();
    void Penumbra();
    void ResultsValid();
    void ScaleIntensity();
    void Translate();
};


