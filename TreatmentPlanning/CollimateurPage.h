#ifndef COLLIMATEURPAGE_H
#define COLLIMATEURPAGE_H

#include <QObject>
#include <QDialog>
#include <vector>

#include "Collimator.h"

class MTP;

class CollimateurPage : public QObject
{
    Q_OBJECT

public:
     CollimateurPage(MTP* parent);
    ~CollimateurPage();

public slots:
     void SaveToDatabase();
     void GetFromDatabase();
     void ShowFraiseuseProgram();

     void UploadEyePlanCoordinates();
     void UpdateCollimator();

private:
    void SetupDefaults();
    void SetupLineEditValidators();
    void ConnectSignals();

    void DrawCollimator(const Collimator& collimator);
    void WriteCollimatorData();
    bool QueryUserForFile(QString default_dir, QString *filename);
    void ClearPatientDatabaseName();
    void WritePatientDatabaseName();

    MTP* parent_;
    QSettings settings_;
    Collimator collimator_;
};

#endif
