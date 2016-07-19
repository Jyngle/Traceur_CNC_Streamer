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

//BOUTONS
float last_timer = 0;
QTime timer;
int flip = 0;
int flip2 = 0;
//BOUTONS

void anti_rebond(){
    if ((timer.elapsed() - last_timer) >= 200){
        Menu::pause();
        last_timer = timer.elapsed();
    }
}

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

    //BOUTONS IMPLEMENTATION
        timer.start();
        wiringPiSetup();
        pinMode (0, INPUT) ;
        pinMode (1, INPUT) ;
        pinMode (2, INPUT) ;
        pinMode (3, INPUT) ;
        wiringPiISR(0,INT_EDGE_RISING,anti_rebond);
        QStringList liste;
        QStringList liste_output;
    //BOUTONS IMPLEMENTATION

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

        if (flip == 1){
                  qDebug() << "Pause";
                  QThread::msleep(1000);
                  flip2 = 1;
      }

        if (flip2 ==1){
                   flip2 = 0;
                   QThread::msleep(1000);
                   qDebug()<<"reprise du cycle";
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
      if (flip == 0){
    flip = 1;}
      else if (flip ==1){
       flip = 0;}
}

