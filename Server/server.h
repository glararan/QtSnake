#ifndef SERVER_H
#define SERVER_H

#include "opcodes.h"

#include <QTcpServer>
#include <QVector>
#include <QTcpSocket>
#include <QPair>

#include <QDebug>

struct Client
{
    QHostAddress peerAddr;
    quint16      peerAppPort;

    Client(QHostAddress peerAddress, quint16 peerPort)
    {
        peerAddr    = peerAddress;
        peerAppPort = peerPort;
    }
};

struct SnakeData
{
    QVector<QPoint> body;

    QPoint food;

    SnakeData(QVector<QPoint> snakeBody, QPoint snakeFood)
    {
        body = snakeBody;
        food = snakeFood;
    }

    SnakeData()
    {
        body = QVector<QPoint>();
        food = QPoint();
    }
};

class SnakeDataClass
{
public:
    SnakeDataClass()
    {
    }

    void setData(SnakeData& snakeData)
    {
        data.body = snakeData.body;
        data.food = snakeData.food;
    }

    SnakeData data;
};

class Server : public QTcpServer
{
    Q_OBJECT

public:
    Server(QObject* parent = 0);
    ~Server();

private:
    struct Packet
    {
        Opcodes opcode;

        QString description;

        QByteArray data;

        Packet(Opcodes op, QString& descr, QByteArray& d)
        {
            opcode      = op;
            description = descr;
            data        = d;
        }
    };

    Packet toPacket(QByteArray& packet);

    void broadcastPacket(const Opcodes packetType, QTcpSocket* sender);

    QByteArray toData(const int data);
    QByteArray toData(Client client, SnakeData& data);
    QByteArray toData(SnakeData& data);
    QByteArray toData(const QTcpSocket* client);

    QByteArray packetToData(const Packet packet);

    QVector<QTcpSocket*>                    clients;
    QVector<QPair<QTcpSocket*, SnakeDataClass>> clientsData;

private slots:
    void acceptConnection();
    void startRead();
    void disconnected();
};

inline QDataStream& operator>>(QDataStream& in, Opcodes& opcode)
{
    int buffer;

    in >> buffer;

    opcode = Opcodes(buffer);

    return in;
}

inline QDataStream& operator<<(QDataStream& out, Client& socket)
{
    out << socket.peerAddr
        << socket.peerAppPort;

    return out;
}

inline QDataStream& operator>>(QDataStream& in, SnakeData& data)
{
    QVector<QPoint> body;
    QPoint          food;

    in >> body
       >> food;

    data.body = body;
    data.food = food;

    return in;
}

inline QDataStream& operator<<(QDataStream& out, SnakeData& data)
{
    out << data.body
        << data.food;

    return out;
}

inline bool operator==(const SnakeData& data1, const SnakeData& data2)
{
    return data1.body == data2.body && data1.food == data2.food;
}

#endif // SERVER_H
