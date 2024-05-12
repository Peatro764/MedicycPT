#ifndef DATAREADER_H
#define DATAREADER_H

#include <QTextStream>
#include <QDataStream>
#include <QTimer>
#include <QByteArray>
#include <QObject>
#include <QSettings>

class DataReader : public QObject
{
    Q_OBJECT

public:
    virtual void Read() = 0;
    virtual void Abort() = 0;

signals:
    void ProgramRead(QString program);
    void ReadError(QString error);
};

#endif
