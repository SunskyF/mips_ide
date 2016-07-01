#-------------------------------------------------
#
# Project created by QtCreator 2016-03-31T17:49:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mips_ide
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mips.cpp \
    mips_utl.cpp \
    mipscoe.cpp

HEADERS  += mainwindow.h \
    mips.h \
    mips_utl.h \
    mipscoe.h

FORMS    += mainwindow.ui
