#include <QtTest/QtTest>

class TestCompensateur : public QObject {
    Q_OBJECT
private slots:
    void Constructor();
    void Comparisons();
    void CorrectBaseWidth();
    void CorrectPlexiglasColumnHeight();
    void CorrectTheoreticalCurveHeight();
    void StepCurveWithinError();
    void FraiseuseFormat();
};
