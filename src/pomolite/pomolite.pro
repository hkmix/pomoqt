#-------------------------------------------------
#
# Project created by QtCreator 2016-11-19T12:39:37
#
#-------------------------------------------------

QT       -= core gui

TEMPLATE = lib

INCLUDEPATH += . ..

SOURCES += pomolite.cc

HEADERS += pomolite.h \
           pomoresult.h

LIBS += -lsqlite3

DISTFILES += doc/database.txt
