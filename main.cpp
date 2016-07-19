#include <QCoreApplication>
#include <QDebug>
#include "menu.h"
#include <QThread>

int main(int argc, char *argv[])
{     
    QCoreApplication a(argc, argv);
    Menu menu;

    switch (argc){

    case 2:
    {
        QString gcode(argv[1]);
        menu.read_file(gcode);
        break;
    }
    default:
        menu.show();
        break;

    }

    return 0;
}
