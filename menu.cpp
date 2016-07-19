#include "menu.h"
#include "serial.h"
#include <stdio.h>
#include <QFile>
#include <QDebug>
#include <QCoreApplication>
#include <QThread>
#include <QUrl>
#include <QtNetwork/QtNetwork>
#include <QTime>
#include <wiringPi.h>

Serial serial;

void Menu::show(){

    QTextStream stream(stdin);
    QString port_COM;
    QString Gcode;
    QString rep_menu;
    QString filename = QCoreApplication::applicationDirPath() + "/gcode.nc";
    QFile fichier_Gcode(filename);
    fichier_Gcode.open(QIODevice::ReadOnly |QIODevice::Text);
    QTextStream fichier_in(&fichier_Gcode);

    qDebug() << "Raspberry PI interface Gcode" << endl;
    printf("Port COM : ");
    port_COM = stream.readLine();

    serial.set_COM(); //ouverture du port

    while(true){

    qDebug() << "Menu : ";
    qDebug() << "Mode Manuel (ligne par ligne) : 0";
    qDebug() << "Mode Automatique (fichier Gcode) : 1";
    printf("Reponse : ");
    rep_menu = stream.readLine();

    if (rep_menu == "0"){
        do{
        printf("Envoi Gcode : ");
        Gcode = stream.readLine();
        serial.send_rep_COM(Gcode);
        //send_php(serial.send_rep_COM("?"));
        }while(Gcode != "exit");
    }
     else if (rep_menu == "1"){
        if(!fichier_Gcode.exists())
           {
               qDebug() << "Probleme a la lecture du fichier";
           }
        else
        {

        QString ligne = fichier_in.readLine();
        serial.send_rep_COM(ligne);

        while (!ligne.isNull())
        {
            ligne = fichier_in.readLine();
            serial.send_rep_COM(ligne);
            //send_php(serial.send_rep_COM("?"));
        }

        }
    }
    else qDebug() << "Erreur";
    }
}


void Menu::read_file(QString gcode){

    QString filename = QCoreApplication::applicationDirPath() + "/" + gcode;
    QFile fichier_Gcode(filename);
    fichier_Gcode.open(QIODevice::ReadOnly |QIODevice::Text);
    QTextStream fichier_in(&fichier_Gcode);

    serial.set_COM();

    if(!fichier_Gcode.exists())
       {
           qDebug() << "Probleme a la lecture du fichier";
       }
    else
    {
    QString ligne = fichier_in.readLine();
    serial.send_rep_COM(ligne);
    while (!ligne.isNull())
    {

        ligne = fichier_in.readLine();
        serial.send_rep_COM(ligne);

    }
    }
    fichier_Gcode.close();
}


  void Menu::send_php(QString ligne){

      QNetworkAccessManager *manager = new QNetworkAccessManager();

      QString str("http://localhost/interface_cnc/getpos.php");

      QNetworkRequest request = QNetworkRequest(QUrl(str));
      request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
      QByteArray postData;
      QString data = "position=";
      data.append(ligne);

      postData.append(data.toLatin1());

      QNetworkReply *r = manager->post(request,postData);

      QEventLoop loop;
      QObject::connect(r, SIGNAL(finished()), &loop, SLOT(quit()));
      loop.exec();

  }



