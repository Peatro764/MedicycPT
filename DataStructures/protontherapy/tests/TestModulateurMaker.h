#include <QtTest/QtTest>

#include "DepthDoseCurve.h"
#include "ModulateurMaker.h"

class TestModulateurMaker : public QObject {
    Q_OBJECT
private:

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    std::vector<DepthDoseCurve> GetDepthDoseCurves(int nmb);

    void GetRequiredUmPlexiStepsNoDegradeur();
    void GetRequiredUmPlexiStepsWithDegradeur();
    void GetRequiredUmPlexiStepsWithDegradeurAndDecalage();
    void GetSectorWeights();
    void GetWeightedCurves();
    void GetSumOfWeightedCurves();
    void IterateWeights();
};


