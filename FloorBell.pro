#-------------------------------------------------
#
# Project created by QtCreator 2020-08-02T15:34:48
#
#-------------------------------------------------

QT       += core gui sql serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets \
sql

TARGET = floorbell
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

DISTFILES += \
    myDB.db
