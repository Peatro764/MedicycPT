#ifndef DBWorker_H
#define DBWorker_H

#include <QObject>

#include "PTRepo.h"

class DBWorker : public QObject {
  Q_OBJECT

   public:
    DBWorker(QObject* parent = 0);
    ~DBWorker();

signals:
    void SIGNAL_Patient(Patient patient);
    void SIGNAL_Error(QString message);

public slots:
    void Setup();
    void SaveXRayRecord(int dossier, double peak_rate, double integrated_charge);
    void GetPatient(int dossier);

private slots:

private:
    std::unique_ptr<QSettings> settings_;
    std::unique_ptr<PTRepo> repo_;
    bool emit_messages_ = true;
};


#endif // DBWorker_H
