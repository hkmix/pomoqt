#-------------------------------------------------
#
# Project created by QtCreator 2016-11-19T12:36:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pomoqt
TEMPLATE = app

INCLUDEPATH += . $$PWD/..

SOURCES += main.cc\
           pomo_window.cc

HEADERS  += pomo_window.h

FORMS    += pomo_window.ui

LIBS += -L../pomolite -lpomolite
