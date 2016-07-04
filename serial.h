#ifndef SERIAL_H
#define SERIAL_H

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>
#include <QCoreApplication>

class Serial
{    

public:
    void set_COM(QString port_COM);
    QString send_rep_COM(QString str);

private:
    QSerialPort serial;

};


#endif // SERIAL_H
