QT += core
QT -= gui
QT += serialport
QT += network

CONFIG += c++11

TARGET = /home/arthur/Documents/ProjetQT/build-streamer_gcode-Rpi-Debug/streamer_gcode

target.path = /home/pi/test/
INSTALLS += target

INCLUDEPATH += /usr/local/include
LIBS += -L/usr/local/lib/ -lwiringPi


CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    serial.cpp \
    menu.cpp

HEADERS += \
    serial.h \
    menu.h
