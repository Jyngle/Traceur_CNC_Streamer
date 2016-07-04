#include <QCoreApplication>
#include <QDebug>
#include "menu.h"
#include <QThread>

int main(int argc, char *argv[])
{     
    QCoreApplication a(argc, argv);
    Menu menu;

    switch (argc){

    case 3:
    {
        QString port(argv[1]);
        QString gcode(argv[2]);
        menu.read_file(port,gcode);
        break;
    }
    default:
        menu.show();
        break;

    }

    return 0;
}
