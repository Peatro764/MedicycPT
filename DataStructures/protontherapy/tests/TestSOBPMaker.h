#include <QtTest/QtTest>

#include "SOBPMaker.h"

class TestSOBPMaker : public QObject {
    Q_OBJECT
private:

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void GetSOBP_NoInterpolation_NoDegradeur();
    void GetSOBP_NoInterpolation_ExternalDegradeur();
    void GetSOBP_NoInterpolation_InternalPlusExternalDegradeur();

    void GetSOBP_WithInterpolation();
    void GetSOBP_WithExtrapolation();
};


