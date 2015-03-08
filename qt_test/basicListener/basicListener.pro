#-------------------------------------------------
#
# Project created by QtCreator 2015-01-11T17:53:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = basicListener
TEMPLATE = app

INCLUDEPATH +=  ../../commandline/hidapi/hidapi/
INCLUDEPATH +=  ../../commandline/t100_lib/

macx: SOURCES += ../../commandline/hidapi/mac/hid.c
macx: LIBS += -framework CoreFoundation -framework IOkit

SOURCES += ../../commandline/t100_lib/t100_lib.cpp \
    t100Helper.cpp \
    TableModel.cpp

SOURCES += main.cpp\
        mainwindow.cpp \

HEADERS  += mainwindow.h \
    t100Helper.h \
    TableModel.h

FORMS    += mainwindow.ui
