#include <QtTest/QtTest>

class TestCollimateur : public QObject {
    Q_OBJECT
private slots:
    void Constructors();
    void InternalEdgesPathFormat();
    void SetDossier();
    void SetLastName();
    void SetFirstName();
    void IsValid();
    void ReadEyePlanFile();
};
