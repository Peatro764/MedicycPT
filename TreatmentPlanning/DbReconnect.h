#ifndef TREATMENTPLANNING_DBRECONNECT_H
#define TREATMENTPLANNING_DBRECONNECT_H

#include <QObject>
#include <QMessageBox>
#include <QSettings>


class DbReconnect : public QObject
{
    Q_OBJECT

public:
    DbReconnect();
    ~DbReconnect();

signals:
    void ReconnectDb();

};

#endif
