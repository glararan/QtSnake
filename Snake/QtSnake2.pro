#-------------------------------------------------
#
# Project created by QtCreator 2014-04-08T21:29:20
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtSnake2
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    surface.cpp \
    snake.cpp \
    client.cpp

HEADERS  += mainwindow.h \
    surface.h \
    snake.h \
    client.h

FORMS    += mainwindow.ui \
    surface.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/ -lQt5PlatformSupport
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/ -lQt5PlatformSupportd
else:unix: LIBS += -L$$PWD/ -lQt5PlatformSupport

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/libQt5PlatformSupport.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/libQt5PlatformSupportd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/Qt5PlatformSupport.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/Qt5PlatformSupportd.lib
else:unix: PRE_TARGETDEPS += $$PWD/libQt5PlatformSupport.a
