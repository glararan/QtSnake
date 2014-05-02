#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>

#include <QTcpSocket>
#include <QNetworkSession>

enum Opcodes
{
    // Hi
    Hello    = 0x00,

    // Snake Broadcast
    BroadcastSnake = 0x10,

    // Snake
    SnakeCorrection = 0x11,

    // Player
    PlayerAdd    = 0x20,
    PlayerRemove = 0x21,

    // G.O.
    GameOver = 0xff
};

struct PlayerClient
{
    QHostAddress peerAddr;
    quint16      peerAppPort;

    PlayerClient(QHostAddress peerAddress, quint16 peerPort)
    {
        peerAddr    = peerAddress;
        peerAppPort = peerPort;
    }

    PlayerClient()
    {
        peerAddr    = QHostAddress();
        peerAppPort = 0;
    }

    const bool isNull() const
    {
        if(peerAddr.isNull() && peerAppPort == 0)
            return true;

        return false;
    }

    inline bool operator==(const PlayerClient& second) const
    {
        if(peerAddr == second.peerAddr
        && peerAppPort == second.peerAppPort)
            return true;

        return false;
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

class Client : public QObject
{
    Q_OBJECT

public:
    Client(QString& serverIP, quint16 serverPort, QObject* parent = 0);
    ~Client();

    bool connectToServer();

    void sendPacket(Opcodes opcode, QString& description, QByteArray& data);

    QByteArray toData(const int data);
    QByteArray toData(SnakeData& data);

private:
    QTcpSocket* tcpSocket;

    QNetworkSession* networkSession;

    QString server;

    quint16 port;

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

    QByteArray packetToData(const Packet packet);

private slots:
    void readRead();
    void displayError(QAbstractSocket::SocketError socketError);
    void sessionOpened();

signals:
    void error(QString error);

    void broadcastSnake(PlayerClient& pClient, SnakeData& data);

    void snakeCorrection(SnakeData& data);

    void newPlayer(PlayerClient& pClient);
    void removePlayer(PlayerClient& pClient);
};

inline QDataStream& operator>>(QDataStream& in, Opcodes& opcode)
{
    int buffer;

    in >> buffer;

    opcode = Opcodes(buffer);

    return in;
}

inline QDataStream& operator>>(QDataStream& in, PlayerClient& socket)
{
    QHostAddress pAddr = QHostAddress();
    quint16      pPort = 0;

    in >> pAddr
       >> pPort;

    socket.peerAddr    = pAddr;
    socket.peerAppPort = pPort;

    return in;
}

inline QDataStream& operator>>(QDataStream& in, SnakeData& data)
{
    QVector<QPoint> body = QVector<QPoint>();
    QPoint          food = QPoint();

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

#endif // CLIENT_H