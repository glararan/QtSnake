#include <QApplication>

#include <QTime>

#include "mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    qsrand((uint)QTime::currentTime().msec());

    MainWindow mw;
    mw.show();

    return a.exec();
}