#ifndef MENU_H
#define MENU_H

#include <QString>

class Menu
{

public:
    void show();
    void read_file(QString gcode);
    void send_php(QString ligne);
    static void pause();
    static QStringList parse_pos(QString pos);


};

#endif // MENU_H
