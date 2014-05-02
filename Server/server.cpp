#include "server.h"

#include <stdlib.h>

#include <QDataStream>
#include <QDebug>

Server::Server(QObject *parent)
: QTcpServer(parent)
{
    connect(this, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
}

Server::~Server()
{
    foreach(QTcpSocket* socket, clients)
    {
        socket->close();

        delete socket;
    }

    close();
}

void Server::acceptConnection()
{
    QTcpSocket* client = nextPendingConnection();

    connect(client, SIGNAL(readyRead()),    this, SLOT(startRead()));
    connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));

    qDebug() << "New client from:" << client->peerAddress().toString() << ":" << client->peerPort() << "... total clients:" << (clients.count() + 1);

    // Send broadcast packet
    broadcastPacket(PlayerAdd, client);

    // Add already in-game clients
    foreach(QTcpSocket* player, clients)
    {
        Packet broadcastingPacket = Packet(PlayerAdd, QString("Broadcasting player add"), toData(player));

        client->write(packetToData(broadcastingPacket));
        client->flush();
    }

    clients.append(client);
    clientsData.append(qMakePair<QTcpSocket*, SnakeDataClass>(client, SnakeDataClass()));
}

void Server::startRead()
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());

    Packet packet = toPacket(client->readAll());

    switch(packet.opcode)
    {
        case Hello:
        case GameOver:
            {
                int packetData;

                QDataStream ds(&packet.data, QIODevice::ReadOnly);
                ds.setVersion(QDataStream::Qt_5_2);
                ds >> packetData;

                qDebug() << "Client" << client->peerAddress().toString() << "." << client->peerPort() << ":" << packet.description << packetData;

                if(packet.opcode == GameOver)
                    client->disconnectFromHost();
            }
            break;

        case BroadcastSnake:
            {
                SnakeData packetData;

                QDataStream ds(&packet.data, QIODevice::ReadWrite);
                ds.setVersion(QDataStream::Qt_5_2);
                ds >> packetData;

                //qDebug() << "Broadcasting snake from Client" << client->peerAddress().toString() << ":" << client->peerPort();

                // set data to server side
                for(int i = 0; i < clientsData.count(); ++i)
                {
                    if(clientsData[i].first == client)
                    {
                        clientsData[i].second.setData(packetData);

                        break;
                    }
                }

                // check for another snake bite
                for(int i = 0; i < clientsData.count(); ++i)
                {
                    QTcpSocket* socket = clientsData.at(i).first;

                    if(socket != client)
                    {
                        if(clientsData.at(i).second.data.body.count() < 1)
                            continue;

                        // if head reach some point
                        for(int j = 0; j < packetData.body.count(); ++j)
                        {
                            if(clientsData.at(i).second.data.body.first() == packetData.body[j])
                            {
                                // resize
                                packetData.body.resize(j);

                                // send packet about snake decrease
                                Packet returningPacket = Packet(SnakeCorrection, QString("Snake correction"), toData(packetData));

                                client->write(packetToData(returningPacket));
                                client->flush();

                                break;
                            }
                        }
                    }
                }

                // broadcast
                Client pClient = Client(client->peerAddress(), client->peerPort());

                Packet broadcastingPacket = Packet(BroadcastSnake, QString("Broadcasting snake"), toData(pClient, packetData));

                foreach(QTcpSocket* broadcastClient, clients)
                {
                    if(broadcastClient != client)
                    {
                        broadcastClient->write(packetToData(broadcastingPacket));
                        broadcastClient->flush();
                    }
                }
            }
            break;
    }
}

void Server::disconnected()
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());

    for(int i = 0; i < clients.count(); i++)
    {
        if(clients[i] == client)
        {
            clients.removeAt(i);

            break;
        }
    }

    for(int i = 0; i < clientsData.count(); i++)
    {
        if(clientsData.at(i).first == client)
        {
            clientsData.removeAt(i);

            break;
        }
    }

    // Send broadcast packet
    broadcastPacket(PlayerRemove, client);

    qDebug() << "Client" << client->peerAddress().toString() << "." << client->peerPort() << ": Has disconnected...total clients:" << clients.count();

    client->deleteLater();
}

void Server::broadcastPacket(const Opcodes packetType, QTcpSocket* sender)
{
    switch (packetType)
    {
        case PlayerAdd:
            {
                foreach(QTcpSocket* client, clients)
                {
                    if(sender != client)
                    {
                        qDebug() << "Broadcasting player add from Client" << sender->peerAddress().toString() << ":" << client->peerPort();

                        Packet broadcastingPacket = Packet(PlayerAdd, QString("Broadcasting player add"), toData(sender));

                        client->write(packetToData(broadcastingPacket));
                        client->flush();
                    }
                }
            }
            break;

        case PlayerRemove:
            {
                foreach(QTcpSocket* client, clients)
                {
                    qDebug() << "Broadcasting player remove from Client" << sender->peerAddress().toString() << ":" << client->peerPort();

                    Packet broadcastingPacket = Packet(PlayerRemove, QString("Broadcasting player remove"), toData(sender));

                    client->write(packetToData(broadcastingPacket));
                    client->flush();
                }
            }
            break;
    }
}

Server::Packet Server::toPacket(QByteArray& packet)
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

QByteArray Server::packetToData(const Server::Packet packet)
{
    QByteArray array;

    QDataStream ds(&array, QIODevice::WriteOnly);
    ds.setVersion(QDataStream::Qt_5_2);
    ds << packet.opcode
       << packet.description
       << packet.data;

    return array;
}

QByteArray Server::toData(const int data)
{
    QByteArray array;

    QDataStream ds(&array, QIODevice::WriteOnly);
    ds.setVersion(QDataStream::Qt_5_2);
    ds << data;

    return array;
}

QByteArray Server::toData(Client client, SnakeData& data)
{
    QByteArray array;

    QDataStream ds(&array, QIODevice::ReadWrite);
    ds.setVersion(QDataStream::Qt_5_2);
    ds << client
       << data;

    return array;
}

QByteArray Server::toData(SnakeData& data)
{
    QByteArray array;

    QDataStream ds(&array, QIODevice::ReadWrite);
    ds.setVersion(QDataStream::Qt_5_2);
    ds << data;

    return array;
}

QByteArray Server::toData(const QTcpSocket* client)
{
    QByteArray array;

    QDataStream ds(&array, QIODevice::WriteOnly);
    ds.setVersion(QDataStream::Qt_5_2);
    ds << Client(client->peerAddress(), client->peerPort());

    return array;
}