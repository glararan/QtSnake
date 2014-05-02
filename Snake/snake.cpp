#include "snake.h"

Snake::Snake(QPoint& start, QPoint& gridSize, Direction dir, QColor& bodyColor, QColor& ballColor, Client* cParent, QObject* parent)
: QObject(parent)
, grid(gridSize)
, direction(dir)
, color(bodyColor)
, foodColor(ballColor)
, dirChanged(false)
, client(cParent)
, pClient(PlayerClient())
{
    body.append(start);
}

Snake::Snake(QPoint& start, QPoint& gridSize, Direction dir, QColor& bodyColor, QColor& ballColor, PlayerClient& cParent, QObject* parent)
: QObject(parent)
, grid(gridSize)
, direction(dir)
, color(bodyColor)
, foodColor(ballColor)
, dirChanged(false)
, client(NULL)
{
    body.append(start);

    pClient = PlayerClient(cParent.peerAddr, cParent.peerAppPort);
}

Snake::~Snake()
{
}

void Snake::update()
{
    if(getIsDead())
    {
        emit snakeIsDead();

        return;
    }

    if(getHead().x() <= -1 || getHead().x() >= grid.x() || getHead().y() <= -1 || getHead().y() >= grid.y())
    {
        emit killedSelf();

        return;
    }

    for(int i = 0; i < body.count(); ++i)
    {
        if(i == 0)
        {
            last = body.first();

            switch(direction)
            {
                case Top:
                    body.first().setY(body.first().y() - 1);
                    break;

                case Bottom:
                    body.first().setY(body.first().y() + 1);
                    break;

                case Left:
                    body.first().setX(body.first().x() - 1);
                    break;

                case Right:
                    body.first().setX(body.first().x() + 1);
                    break;
            }
        }
        else
        {
            QPoint now = body[i];

            if(body[i] == body.first())
                emit killedSelf();

            body[i] = last;

            last = now;
        }
    }

    dirChanged = false;
}

int Snake::random(int low, int high)
{
    return qrand() % ((high + 1) - low) + low;
}

void Snake::generateFood()
{
    int x = random(0, grid.x() - 1);
    int y = random(0, grid.y() - 1);

    bool okay = true;

    while(true)
    {
        okay = true;

        foreach(QPoint snakeBody, body)
        {
            if(snakeBody.x() == x && snakeBody.y() == y)
            {
                okay = false;

                break;
            }
        }

        if(okay)
            break;
        else
        {
            x = random(0, grid.x() - 1);
            y = random(0, grid.y() - 1);
        }
    }

    foodPosition = QPoint(x, y);
}

void Snake::addBody()
{
    update();

    body.append(last);
}