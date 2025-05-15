# mainwindow.pro
QT += core gui widgets network
CONFIG += c++11
TARGET = mainwindow
TEMPLATE = app

SOURCES += main.cpp \
           mainwindow.cpp

HEADERS += mainwindow.h

FORMS += mainwindow.ui
