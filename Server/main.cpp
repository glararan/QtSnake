#include <QCoreApplication>

#include <QTime>
#include <QTcpSocket>
#include <QDebug>

#include "server.h"

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "            ";
    qDebug() << "            ";
    qDebug() << "          #     # ##### ##### #####  ###  #   #    #    ";
    qDebug() << "          #     # #       #   #     #   # #  #     #    ";
    qDebug() << "           #   #  #       #   #     #     # #     # #   ";
    qDebug() << "           #   #  #####   #   #####  ###  ###     # #   ";
    qDebug() << "           #   #  #       #   #         # #  #   #   #  ";
    qDebug() << "            # #   #       #   #     #   # #  #   #####  ";
    qDebug() << "            # #   #       #   #     #   # #   # #     # ";
    qDebug() << "             #    #####   #   #####  ###  #   # #     # ";
    qDebug() << "                                                        ";
    qDebug() << "                                                                           ";
    qDebug() << "   ###  ##   #    #    #   # #####    ###  ##### ####  #     # ##### ####  ";
    qDebug() << "  #   # ##   #    #    #  #  #       #   # #     #   # #     # #     #   # ";
    qDebug() << "  #     # #  #   # #   # #   #       #     #     #   #  #   #  #     #   # ";
    qDebug() << "   ###  # #  #   # #   ###   #####    ###  ##### ####   #   #  ##### ####  ";
    qDebug() << "      # #  # #  #   #  #  #  #           # #     #   #  #   #  #     #   # ";
    qDebug() << "  #   # #  # #  #####  #  #  #       #   # #     #   #   # #   #     #   # ";
    qDebug() << "  #   # #   ## #     # #   # #       #   # #     #   #   # #   #     #   # ";
    qDebug() << "   ###  #   ## #     # #   # #####    ###  ##### #   #    #    ##### #   # ";

    qDebug() << "\n";

    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));

    // Get local IP Address
    QString ip;

    QTcpSocket dnsTestSocket;

    // Test connect to google dns
    dnsTestSocket.connectToHost("8.8.8.8", 53);

    if(dnsTestSocket.waitForConnected(3000))
        ip = dnsTestSocket.localAddress().toString();
    else
    {
        qDebug() << "Couldn't resolve local IP Address...";

        return 0;
    }

    quint16 port = 47777;

    Server server;

    if(!server.listen(QHostAddress(ip), port))
    {
        qDebug() << "Critical: Unable to start the server" << server.errorString();

        return 0;
    }

    qDebug() << "IP Address:" << ip;
    qDebug() << "Port:" << server.serverPort();
    qDebug() << "\n";

    return a.exec();
}