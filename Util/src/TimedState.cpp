#include "TimedState.h"

#include <QDebug>

namespace utils {

TimedState::TimedState(QState *parent, int timeout_ms, QString message)
    : QState(parent) {
    timeout_timer_.setInterval(timeout_ms);
    timeout_timer_.setSingleShot(true);

    QObject::connect(&timeout_timer_, &QTimer::timeout, this, [&, message]() { emit SIGNAL_Timeout(message); });
    this->addTransition(&timeout_timer_, &QTimer::timeout, this);
}

TimedState::TimedState(QState *parent, int timeout_ms, QString message, QAbstractState *error_state)
    : QState(parent) {
    timeout_timer_.setInterval(timeout_ms);
    timeout_timer_.setSingleShot(true);

    QObject::connect(&timeout_timer_, &QTimer::timeout, this, [&, message]() { emit SIGNAL_Timeout(message); });
    this->addTransition(&timeout_timer_, &QTimer::timeout, error_state);
}

void TimedState::onEntry(QEvent *event) {
//    qDebug() << "TimedState::onEntry";
    (void)event;
    timeout_timer_.start();
}

void TimedState::onExit(QEvent *event) {
//    qDebug() << "TimedState::onExit";
    (void)event;
    timeout_timer_.stop();
}

void TimedState::Reset() {
    timeout_timer_.start();
}

}
