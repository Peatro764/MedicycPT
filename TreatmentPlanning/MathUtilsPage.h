#ifndef MATHUTILSPAGE_H
#define MATHUTILSPAGE_H

#include <QObject>
#include <QDialog>
#include <vector>

class MTP;

class MathUtilsPage : public QObject
{
    Q_OBJECT

public:
     MathUtilsPage(MTP* parent);
    ~MathUtilsPage();

public slots:
     void CalculateNIF();
     void CalculateCOORD();

private:
    void SetupDefaults();
    void SetupLineEditValidators();
    void ConnectSignals();

    MTP* parent_;
    const double nif_dcp = 107.5;
};

#endif
