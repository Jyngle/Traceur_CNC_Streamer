#include "serial.h"
#include <QDebug>
#include <QThread>

void Serial::set_COM(QString port_COM)
{
    QSerialPortInfo info(port_COM);
        // Check info sur le port
    qDebug() << "Nom        : " << info.portName();
    qDebug() << "Manufacturer: " << info.manufacturer();
    qDebug() << "Busy: " << info.isBusy() << endl;

    serial.setPortName(port_COM);
    serial.open(QIODevice::ReadWrite);
    serial.setBaudRate(QSerialPort::Baud115200);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);
    QThread::msleep(500);
    serial.write("\n");
    while(!serial.waitForReadyRead(50)){}
    serial.readAll();
    serial.clear();
}

QString Serial::send_rep_COM(QString msg)
{
    if (serial.isOpen() && serial.isWritable())
                {
                    QByteArray ba = msg.toLocal8Bit(); //conversion QString -> QByteArray
                    if (msg != "?"){
                    ba.append("\r");
                    serial.clear();
                    serial.write(ba);}

                    else {
                        serial.clear();
                        serial.write(ba);}

                    qDebug() << "Envoi OK : " << ba <<  endl;

                        while(!serial.waitForReadyRead(10)){};

                        QString str;
                        QByteArray response = serial.readAll();
                        str = QString(response);

                        while(!str.contains("\n")){
                        serial.waitForReadyRead(10);
                        response += serial.readAll();
                        str = QString(response);}

                        str.remove(str.size()-2,str.size());

                        qDebug() << str << endl;

                     return str;

                }
    else
                {
                    qDebug() << "ERREUR de Port COM" << endl;
                    return "ERREUR de Port COM";

                }
}
