#include "client.h"

#include <QtNetwork>
#include <QMessageBox>

Client::Client(QString& serverIP, quint16 serverPort, QObject* parent)
: QObject(parent)
, networkSession(NULL)
, server(serverIP)
, port(serverPort)
{
    tcpSocket = new QTcpSocket(this);

    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readRead()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));

    QNetworkConfigurationManager manager;

    if(manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired)
    {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));

        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();

        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);

        if((config.state() & QNetworkConfiguration::Discovered) != QNetworkConfiguration::Discovered)
            config = manager.defaultConfiguration();

        networkSession = new QNetworkSession(config, this);

        connect(networkSession, SIGNAL(opened()), this, SLOT(sessionOpened()));

        networkSession->open();
    }
}

Client::~Client()
{
    tcpSocket->close();

    delete tcpSocket;
}

bool Client::connectToServer()
{
    tcpSocket->abort();

    tcpSocket->connectToHost(server, port);

    return tcpSocket->waitForConnected(5000);
}

void Client::sendPacket(Opcodes opcode, QString& description, QByteArray& data)
{
    if(tcpSocket->state() != QAbstractSocket::ConnectedState && !connectToServer())
    {
        qFatal("Couldn't connect to server");

        return;
    }

    Packet packet = Packet(opcode, description, data);

    tcpSocket->write(packetToData(packet));
    tcpSocket->flush();
}

void Client::readRead()
{
    Packet packet = toPacket(tcpSocket->readAll());

    switch(packet.opcode)
    {
        case BroadcastSnake:
            {
                PlayerClient pClient;

                SnakeData packetData;

                QDataStream ds(&packet.data, QIODevice::ReadWrite);
                ds.setVersion(QDataStream::Qt_5_2);
                ds >> pClient
                   >> packetData;

                emit broadcastSnake(pClient, packetData);
            }
            break;

        case SnakeCorrection:
            {
                SnakeData packetData;

                QDataStream ds(&packet.data, QIODevice::ReadWrite);
                ds.setVersion(QDataStream::Qt_5_2);
                ds >> packetData;

                emit snakeCorrection(packetData);
            }
            break;

        case PlayerAdd:
            {
                PlayerClient pClient;

                QDataStream ds(&packet.data, QIODevice::ReadOnly);
                ds.setVersion(QDataStream::Qt_5_2);
                ds >> pClient;

                emit newPlayer(pClient);
            }
            break;

        case PlayerRemove:
            {
                PlayerClient pClient;

                QDataStream ds(&packet.data, QIODevice::ReadOnly);
                ds.setVersion(QDataStream::Qt_5_2);
                ds >> pClient;

                emit removePlayer(pClient);
            }
            break;
    }
}

void Client::displayError(QAbstractSocket::SocketError socketError)
{
    switch(socketError)
    {
        case QAbstractSocket::RemoteHostClosedError:
            break;

        case QAbstractSocket::HostNotFoundError:
            emit error("The host was not found. Please check the host name and port settings.");
            break;

        case QAbstractSocket::ConnectionRefusedError:
            emit error("The connection was refused by the peer. Make sure the fortune server is running, and check that the host name and port settings are correct.");
            break;

        default:
            emit error(QString("The following error occurred: %1.").arg(tcpSocket->errorString()));
    }
}

void Client::sessionOpened()
{
    // Save the used configuration
    QNetworkConfiguration config = networkSession->configuration();

    QString id;

    if(config.type() == QNetworkConfiguration::UserChoice)
        id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
    else
        id = config.identifier();

    QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
    settings.beginGroup(QLatin1String("QtNetwork"));
    settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
    settings.endGroup();
}

QByteArray Client::toData(const int data)
{
    QByteArray array;

    QDataStream ds(&array, QIODevice::WriteOnly);
    ds.setVersion(QDataStream::Qt_5_2);
    ds << data;

    return array;
}

QByteArray Client::toData(SnakeData& data)
{
    QByteArray array;

    QDataStream ds(&array, QIODevice::ReadWrite);
    ds.setVersion(QDataStream::Qt_5_2);
    ds << data;

    return array;
}

Client::Packet Client::toPacket(QByteArray& packet)
{
    Opcodes opcode;

    QString description;

    QByteArray data;

    QDataStream ds(&packet, QIODevice::ReadOnly);
    ds.setVersion(QDataStream::Qt_5_2);
    ds >> opcode
       >> description
       >> data;

    return Packet(opcode, description, data);
}

QByteArray Client::packetToData(const Packet packet)
{
    QByteArray array;

    QDataStream ds(&array, QIODevice::WriteOnly);
    ds.setVersion(QDataStream::Qt_5_2);
    ds << packet.opcode
       << packet.description
       << packet.data;

    return array;
}