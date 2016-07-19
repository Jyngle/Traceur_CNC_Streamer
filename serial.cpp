#include "serial.h"
#include <QDebug>
#include <QThread>

void Serial::set_COM()
{
    int nb;
    QList<QSerialPortInfo> liste = QSerialPortInfo::availablePorts();
    for (int i = 0;i<liste.size();i++){
        if (liste[i].manufacturer() == "Arduino (www.arduino.cc)"){
            serial.setPortName(liste[i].portName());
            nb = i;

        }
    }
        // Check info sur le port
    qDebug() << "Nom        : " << liste[nb].portName();
    qDebug() << "Manufacturer: " << liste[nb].manufacturer();
    qDebug() << "Busy: " << liste[nb].isBusy() << endl;

    serial.open(QIODevice::ReadWrite);
    serial.setBaudRate(QSerialPort::Baud115200);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);
    QThread::msleep(2000);
    serial.waitForReadyRead(50);
    serial.waitForReadyRead(50);
    serial.waitForReadyRead(50);
    serial.clear();
}

QString Serial::send_rep_COM(QString msg)
{
    if (serial.isOpen() && serial.isWritable())
                {
                    QByteArray ba = msg.toLocal8Bit(); //conversion QString -> QByteArray
                    ba.append("\r");
                    serial.clear();
                    serial.write(ba);

                    qDebug() << "Envoi OK : " << ba <<  endl;

                        while(!serial.waitForReadyRead(10)){};

                        QByteArray response = serial.readAll();
                        QString str(response);

                        while(!str.contains("ok")){
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
