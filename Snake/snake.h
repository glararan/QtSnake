#ifndef SNAKE_H
#define SNAKE_H

#include <QObject>

#include <QColor>
#include <QVector>
#include <QPoint>

#include "client.h"

class Snake : public QObject
{
    Q_OBJECT

public:
    enum Direction
    {
        Right  = 0,
        Left   = 1,
        Top    = 2,
        Bottom = 3,
        Count
    };

    Snake(QPoint& start, QPoint& gridSize, Direction dir, QColor& bodyColor, QColor& ballColor, Client* cParent = NULL, QObject* parent = 0);
    Snake(QPoint& start, QPoint& gridSize, Direction dir, QColor& bodyColor, QColor& ballColor, PlayerClient& cParent, QObject* parent = 0);
    ~Snake();

    void update();

    int random(int low, int high);

    void generateFood();

    void addBody();

    /// Set
    void setDirection(const Direction dir)
    {
        if(dirChanged)
            return;

        switch(dir)
        {
            case Top:
                {
                    if(direction == Bottom && body.count() > 1)
                        return;
                }
                break;

            case Bottom:
                {
                    if(direction == Top && body.count() > 1)
                        return;
                }
                break;

            case Right:
                {
                    if(direction == Left && body.count() > 1)
                        return;
                }
                break;

            case Left:
                {
                    if(direction == Right && body.count() > 1)
                        return;
                }
                break;
        }

        direction = dir;

        dirChanged = true;
    }

    void setData(const SnakeData& data)
    {
        if(getClient() == NULL && !getPClient().isNull())
        {
            body         = data.body;
            foodPosition = data.food;
        }
    }

    void setSnakeCorrection(const SnakeData& data)
    {
        if(getClient() != NULL)
            body = data.body;
    }

    /// Get
    const QColor getColor() const { return color; }

    const QPoint getHead() const { return body.first(); }

    const QPoint getFoodPositon() const { return foodPosition; }

    const QPoint getNextPosition() const
    {
        switch(direction)
        {
            case Top:
                return QPoint(body.first().x(), body.first().y() + 1);
                break;

            case Bottom:
                return QPoint(body.first().x(), body.first().y() - 1);
                break;

            case Left:
                return QPoint(body.first().x() + 1, body.first().y());
                break;

            case Right:
                return QPoint(body.first().x() - 1, body.first().y());
                break;

            default:
                return body.first();
                break;
        }
    }

    const QVector<QPoint> getBody() const { return body; }

    const Direction getDirection() const { return direction; }

    const bool getEating() const
    {
        if(getHead() == foodPosition)
            return true;

        return false;
    }

    const QColor& getFoodColor() const { return foodColor; }

    const Client* getClient() const { return client; }

    const PlayerClient& getPClient() const { return pClient; }

    const bool getIsDead()
    {
        if(body.count() < 1)
            return true;

        return false;
    }

    SnakeData getBroadcastData() const
    {
        return SnakeData(getBody(), getFoodPositon());
    }

private:
    // Snake
    QColor color;

    QVector<QPoint> body;

    Direction direction;

    QPoint grid;
    QPoint last;

    bool dirChanged;

    // Food
    QColor foodColor;

    QPoint foodPosition;

    // Client
    Client* client;

    PlayerClient pClient;

signals:
    void killedSelf();

    void snakeIsDead();

    void foodEaten();
};

#endif // SNAKE_H