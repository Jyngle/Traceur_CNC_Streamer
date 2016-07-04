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

    serial.set_COM(port_COM); //ouverture du port

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


void Menu::read_file(QString port, QString gcode){

    //BOUTONS IMPLEMENTATION
    timer.start();
    wiringPiSetup();
    pinMode (0, INPUT) ;
    pinMode (1, INPUT) ;
    pinMode (2, INPUT) ;
    pinMode (3, INPUT) ;
    wiringPiISR(0,INT_EDGE_RISING,anti_rebond);
    QStringList liste;
    //BOUTONS IMPLEMENTATION

    QString filename = QCoreApplication::applicationDirPath() + "/" + gcode;
    QFile fichier_Gcode(filename);
    fichier_Gcode.open(QIODevice::ReadOnly |QIODevice::Text);
    QTextStream fichier_in(&fichier_Gcode);

    serial.set_COM(port);
    QThread::msleep(1000);

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

        if (flip == 1){
            qDebug() << "Pause";
            QThread::msleep(1000);
            flip2 = 1;
            liste = parse_pos(serial.send_rep_COM("?"));
        }

        while(flip ==1){
            while(digitalRead(1) == 0){serial.send_rep_COM("G91\rG0 X50 Y0\rG90");QThread::msleep(100);}
            while(digitalRead(2) == 0){serial.send_rep_COM("G91\rG0 X-50 Y0\rG90");QThread::msleep(100);}
            while(digitalRead(3) == 0){serial.send_rep_COM("G91\rG0 X0 Y50\rG90");QThread::msleep(100);}
        }

        if (flip2 ==1){
            serial.send_rep_COM("G0 X" + liste[1].remove(0, 5) + " Y" + liste[2]);
            serial.send_rep_COM("G1 Z" + liste[3]);
            flip2 = 0;QThread::msleep(1000);
            qDebug()<<"reprise du cycle";
        }


        //send_php(serial.send_rep_COM("?"));

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

 QStringList Menu::parse_pos(QString pos){
    QStringList liste;
    liste = pos.split(',');
    return liste;
 }


