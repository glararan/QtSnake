#ifndef SURFACE_H
#define SURFACE_H

#include <QWidget>

#include <QVector>

#include "client.h"
#include "snake.h"

namespace Ui
{
    class Surface;
}

class Surface : public QWidget
{
    Q_OBJECT

public:
    explicit Surface(Client* user, QWidget* parent = 0);
    ~Surface();

    Client* getClient() const { return client; }

protected:
    void paintEvent(QPaintEvent* event);

    void timerEvent(QTimerEvent* event);

private:
    /// Pointers
    Ui::Surface* ui;

    Client* client;
    Snake*  snake;

    // multiplayer
    QVector<Snake*> players;

    /// Locals
    int columns;
    int rows;
    int pixelPerPoint;

    int timerID;

    bool cheat;

signals:
    void gameOver();

    void foodEaten();

public slots:
    void setSpeed(int speed);

    void keyPress(Qt::Key& key);

private slots:
    void snakeKilledSelf();
    void snakeIsDead();

    void broadcastSnake(PlayerClient& pClient, SnakeData& data);

    void snakeCorrection(SnakeData& data);

    void newPlayer(PlayerClient& pClient);
    void removePlayer(PlayerClient& pClient);
};

#endif // SURFACE_H
