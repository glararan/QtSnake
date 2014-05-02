#-------------------------------------------------
#
# Project created by QtCreator 2014-04-09T14:35:18
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = QtSnakeServer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    server.cpp

HEADERS += \
    server.h \
    opcodes.h
