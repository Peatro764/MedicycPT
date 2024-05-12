#ifndef TIMEDSTATE_H
#define TIMEDSTATE_H

#include <QStateMachine>
#include <QState>
#include <QObject>
#include <QTimer>

namespace utils {

class TimedState : public QState
{
    Q_OBJECT
public:
    TimedState(QState *parent, int timeout_ms, QString message);
    TimedState(QState *parent, int timeout_ms, QString message, QAbstractState *error_state);

protected:
    void onEntry(QEvent *event) override;
    void onExit(QEvent *event) override;

public slots:
    void Reset();

private slots:

signals:
    void SIGNAL_Timeout(QString message);

private:
    QTimer timeout_timer_;
};

}

#endif
