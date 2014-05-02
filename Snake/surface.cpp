#include "surface.h"
#include "ui_surface.h"

#include <QPainter>
#include <QPaintEvent>
#include <QTimerEvent>
#include <QKeyEvent>
#include <QMessageBox>
#include <QDebug>

Surface::Surface(Client* user, QWidget* parent)
: QWidget(parent)
, ui(new Ui::Surface)
, client(user)
, cheat(false)
{
    setFocusPolicy(Qt::StrongFocus);

    ui->setupUi(this);

    setMinimumHeight(400);
    setMinimumWidth(800);

    columns       = (width() - 20) / 20;
    rows          = (height() - 20) / 20;
    pixelPerPoint = 20;

    snake = new Snake(QPoint(columns / 2, rows / 2), QPoint(columns, rows), Snake::Right, QColor(Qt::black), QColor(Qt::green), client);

    connect(snake, SIGNAL(killedSelf()), this,  SLOT(snakeKilledSelf()));
    connect(snake, SIGNAL(snakeIsDead()), this, SLOT(snakeIsDead()));

    snake->generateFood();

    if(client)
    {
        connect(client, SIGNAL(broadcastSnake(PlayerClient&, SnakeData&)), this, SLOT(broadcastSnake(PlayerClient&, SnakeData&)));

        connect(client, SIGNAL(snakeCorrection(SnakeData&)), this, SLOT(snakeCorrection(SnakeData&)));

        connect(client, SIGNAL(newPlayer(PlayerClient&)),    this, SLOT(newPlayer(PlayerClient&)));
        connect(client, SIGNAL(removePlayer(PlayerClient&)), this, SLOT(removePlayer(PlayerClient&)));
    }

    timerID = startTimer(150); // 150
}

Surface::~Surface()
{
    delete ui;

    if(client)
        delete client;

    delete snake;

    if(players.count() > 0)
    {
        foreach(Snake* player, players)
        {
            delete player;

            player = NULL;
        }
    }
}

void Surface::timerEvent(QTimerEvent* event)
{
    if(snake->getIsDead())
    {
        snake->update();

        return;
    }

    if(snake->getEating())
    {
        snake->addBody();
        snake->generateFood();

        emit foodEaten();
    }
    else if(cheat)
    {
        snake->addBody();

        cheat = false;
    }
    else
        snake->update();

    if(client)
        client->sendPacket(BroadcastSnake, QString("Broadcasting snake"), client->toData(snake->getBroadcastData()));

    update(0, 0, columns * pixelPerPoint, rows * pixelPerPoint);

    QWidget::timerEvent(event);
}

void Surface::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // BG
    painter.fillRect(0, 0, (columns) * pixelPerPoint, (rows) * pixelPerPoint, Qt::gray);

    // Client
    if(client)
    {
        foreach(Snake* player, players)
        {
            bool isTherePlayerSnake = false;

            bool first = true;

            foreach(QPoint point, player->getBody())
            {
                if(first)
                {
                    painter.fillRect(point.x() * pixelPerPoint, point.y() * pixelPerPoint,
                                     pixelPerPoint, pixelPerPoint, QColor(Qt::darkBlue));

                    first = false;
                }
                else
                    painter.fillRect(point.x() * pixelPerPoint, point.y() * pixelPerPoint,
                                     pixelPerPoint, pixelPerPoint, QColor(Qt::blue));

                if(player->getFoodPositon() == point)
                    isTherePlayerSnake = true;
            }

            if(!isTherePlayerSnake)
                painter.fillRect(player->getFoodPositon().x() * pixelPerPoint, player->getFoodPositon().y() * pixelPerPoint,
                                 pixelPerPoint, pixelPerPoint, QColor(Qt::magenta));
        }
    }

    bool isThereSnake = false;

    bool first = true;

    // Snake
    foreach(QPoint snakeBody, snake->getBody())
    {
        if(first)
        {
            painter.fillRect(snakeBody.x() * pixelPerPoint, snakeBody.y() * pixelPerPoint,
                             pixelPerPoint, pixelPerPoint, QColor(Qt::darkGreen));

            first = false;
        }
        else
            painter.fillRect(snakeBody.x() * pixelPerPoint, snakeBody.y() * pixelPerPoint,
                             pixelPerPoint, pixelPerPoint, snake->getColor());

        if(snake->getFoodPositon() == snakeBody)
            isThereSnake = true;
    }

    // Food
    if(!isThereSnake)
        painter.fillRect(snake->getFoodPositon().x() * pixelPerPoint, snake->getFoodPositon().y() * pixelPerPoint,
                         pixelPerPoint, pixelPerPoint, snake->getFoodColor());
}

void Surface::setSpeed(int speed)
{
    killTimer(timerID);

    timerID = startTimer(speed);
}

void Surface::snakeKilledSelf()
{
    emit gameOver();

    killTimer(timerID);
}

void Surface::snakeIsDead()
{
    emit gameOver();

    killTimer(timerID);
}

void Surface::broadcastSnake(PlayerClient& pClient, SnakeData& data)
{
    foreach(Snake* player, players)
    {
        if(pClient == player->getPClient())
            player->setData(data);
    }
}

void Surface::snakeCorrection(SnakeData& data)
{
    snake->setSnakeCorrection(data);
}

void Surface::newPlayer(PlayerClient& pClient)
{
    Snake* player = new Snake(QPoint(columns / 2, rows / 2), QPoint(columns, rows), Snake::Right, QColor(Qt::black), QColor(Qt::green), pClient);

    players.append(player);
}

void Surface::removePlayer(PlayerClient& pClient)
{
    for(int i = 0; i < players.count(); ++i)
    {
        Snake* player = players[i];

        if(player->getPClient() == pClient)
        {
            delete player;

            player = NULL;

            players.removeAt(i);

            break;
        }

        qDebug() << pClient.peerAddr << player->getPClient().peerAddr;
    }
}

void Surface::keyPress(Qt::Key& key)
{
    switch(key)
    {
        case Qt::Key_Up:
            snake->setDirection(Snake::Top);
            break;

        case Qt::Key_Down:
            snake->setDirection(Snake::Bottom);
            break;

        case Qt::Key_Left:
            snake->setDirection(Snake::Left);
            break;

        case Qt::Key_Right:
            snake->setDirection(Snake::Right);
            break;

        case Qt::Key_Enter:
        case Qt::Key_Return:
            {
                QMessageBox msg;
                msg.setWindowTitle("?");
                msg.setIcon(QMessageBox::Information);
                msg.setText("Skřítek uvnitř počítače přenesl špatné data mezi procesorem a pamětí ram. Omlouváme se za jeho chybu a bude nahrazen novým skřítkem! Děkujeme za trpělivost");

                msg.exec();
            }
            break;

        case Qt::Key_Insert:
            cheat = true;
            break;
    }
}