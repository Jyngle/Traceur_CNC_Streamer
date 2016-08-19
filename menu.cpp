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
int sleep = 0;

int pause_b = 26;
int haut = 5;
int bas = 6 ;
int droite = 9;
int gauche = 11;
int z_haut = 19;
int z_bas = 13;


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

        wiringPiSetupGpio();
        pinMode (pause_b, INPUT) ;
        pinMode (haut, INPUT) ;
        pinMode (bas, INPUT) ;
        pinMode (droite, INPUT) ;
        pinMode (gauche, INPUT) ;
        pinMode (z_haut, INPUT) ;
        pinMode (z_bas, INPUT) ;

        wiringPiISR(26,INT_EDGE_RISING,pause);
        QStringList liste_output;

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

        if (ligne.contains("OUTPUT")){
              liste_output = ligne.split(" ");
              pinMode(liste_output[1].toInt(),OUTPUT);
              digitalWrite(liste_output[1].toInt(),liste_output[2].toInt());
              ligne = "OUTPUT";
               }

               if (ligne.contains("PAUSE")){
               liste_output = ligne.split(" ");
               QThread::msleep(liste_output[1].toInt());
               ligne = "PAUSE";
               }

               if (ligne.contains("INPUT")){
               liste_output = ligne.split(" ");
               pinMode(liste_output[1].toInt(),INPUT);
               while(digitalRead(liste_output[1].toInt()) != liste_output[2].toInt()){}
               ligne = "INPUT";
       }

        serial.send_rep_COM(ligne);

     if(sleep == 1){
        while (sleep ==1){

            while(!digitalRead(droite)){serial.send_rep_COM("G91 \n G00 X1 Y0 \n G90");}
            while(!digitalRead(gauche)){serial.send_rep_COM("G91 \n G00 X-1 Y0 \n G90");}
            while(!digitalRead(haut)){serial.send_rep_COM("G91 \n G00 X0 Y1 \n G90");}
            while(!digitalRead(bas)){serial.send_rep_COM("G91 \n G00 X0 Y-1 \n G90");}
            while(!digitalRead(z_haut)){serial.send_rep_COM("G91 \n G00 X0 Y0 Z0.1 \n G90");}
            while(!digitalRead(z_bas)){serial.send_rep_COM("G91 \n G00 X0 Y0 Z-0.1 \n G90");}
        }
        serial.send_rep_COM(ligne);
     }

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

  void Menu::pause(){
      qDebug()<<"initiate pause";
      if (sleep == 0){
    sleep = 1;}
      else if (sleep ==1){
       sleep = 0;}
}

