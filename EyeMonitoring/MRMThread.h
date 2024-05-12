#ifndef MRMThread_H
#define MRMThread_H

#include <QThread>
#include <memory>

class MRMThread : public QThread {
  Q_OBJECT

   public:
    MRMThread(QObject* parent = 0);
    ~MRMThread();

public:

protected:
    void run() override;

signals:
    void SIGNAL_Error(QString message);
    void SIGNAL_XRayDetected(double peak_value, double integrated_charge);
    void SIGNAL_IrradiationStarted();
    void SIGNAL_IrradiationStopped();

private:
};


#endif // MRMThread_H
