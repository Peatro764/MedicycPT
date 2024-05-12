#ifndef QUALITYASSURANCE_BEACON_H
#define QUALITYASSURANCE_BEACON_H

#include <QObject>
#include <QDebug>

class Beacon : public QObject
{
    Q_OBJECT
public:    
    Beacon() {}
    ~Beacon() {}

signals:
    void Activated();
    void BroadcastedMessage(QString msg);

public slots:
    void Activate() { emit Activated(); }
    void BroadcastMessage(QString msg) { emit BroadcastedMessage(msg); }
};


#endif
