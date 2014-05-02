#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "client.h"

#include <QMessageBox>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget* parent)
: QMainWindow(parent)
, ui(new Ui::MainWindow)
, score(0)
, surface(NULL)
, IP(QString())
, port(0)
{
    ui->setupUi(this);

    connect(ui->JoinButton, SIGNAL(clicked()), this, SLOT(validateConnection()));

    connect(ui->action_New_game, SIGNAL(triggered()), this, SLOT(newGame()));
    connect(ui->action_exit,     SIGNAL(triggered()), this, SLOT(close()));
}

MainWindow::~MainWindow()
{
    delete ui;

    if(surface)
        delete surface;
}

void MainWindow::updateScore()
{
    ++score;

    ui->statusBar->showMessage(QString("Vaše skóre: %1").arg(score));
}

void MainWindow::gameOver()
{
    if(surface->getClient())
    {
        Client* client = surface->getClient();

        client->sendPacket(GameOver, QString("Game over...score:"), client->toData(score));
    }

    QMessageBox msg;
    msg.setWindowTitle("Hra skončila!");
    msg.setText(QString("Prohráli jste. Vaše skóre: %1").arg(score));
    msg.setIcon(QMessageBox::Information);

    msg.exec();
}

void MainWindow::newGame()
{
    setCentralWidget(NULL);

    if(surface)
        delete surface;

    surface = new Surface(NULL, this);

    setCentralWidget(surface);

    connect(this, SIGNAL(key(Qt::Key&)), surface, SLOT(keyPress(Qt::Key&)));

    connect(surface, SIGNAL(foodEaten()), this, SLOT(updateScore()));
    connect(surface, SIGNAL(gameOver()),  this, SLOT(gameOver()));

    score = -1;

    updateScore();
}

void MainWindow::validateConnection()
{
    if(ui->IPAddress->text().length() < 7)
    {
        QMessageBox msg;
        msg.setWindowTitle("Error");
        msg.setText("Skřítek v počítači přenesl data IP adresy a zjistil, že je prázdná nebo neni úplná.");
        msg.setIcon(QMessageBox::Information);

        msg.exec();

        return;
    }

    if(ui->ServerPort->text().length() < 1)
    {
        QMessageBox msg;
        msg.setWindowTitle("Error");
        msg.setText("Skřítek v počítači přenesl data cílového portu a zjistil, že je port prázdný. Port je třeba vyplnit, aby věděl kam má doručit data");
        msg.setIcon(QMessageBox::Information);

        msg.exec();

        return;
    }

    Client* client = new Client(ui->IPAddress->text(), ui->ServerPort->text().toInt(), this);

    if(client->connectToServer())
    {
        IP   = ui->IPAddress->text();
        port = ui->ServerPort->text().toInt();

        ui->action_rejoin->setEnabled(true);

        connect(ui->action_rejoin, SIGNAL(triggered()), this, SLOT(rejoin()));

        connect(client, SIGNAL(error(QString)), this, SLOT(showError(QString)));

        // Say hello to server
        client->sendPacket(Hello, QString("Hello"), client->toData(47777));

        setCentralWidget(NULL);

        if(surface)
            delete surface;

        surface = new Surface(client, this);

        setCentralWidget(surface);

        connect(this, SIGNAL(key(Qt::Key&)), surface, SLOT(keyPress(Qt::Key&)));

        connect(surface, SIGNAL(foodEaten()), this, SLOT(updateScore()));
        connect(surface, SIGNAL(gameOver()),  this, SLOT(gameOver()));

        score = -1;

        updateScore();
    }
    else
    {
        delete client;

        client = NULL;

        QMessageBox msg;
        msg.setWindowTitle("Error");
        msg.setText("Skřítek nedokázal najít cestu na rozcestí, aby se dostal do dalšího počítače.");
        msg.setIcon(QMessageBox::Information);

        msg.exec();
    }
}

void MainWindow::rejoin()
{
    Client* client = new Client(IP, port, this);

    if(client->connectToServer())
    {
        connect(client, SIGNAL(error(QString)), this, SLOT(showError(QString)));

        // Say hello to server
        client->sendPacket(Hello, QString("Hello"), client->toData(47777));

        setCentralWidget(NULL);

        if(surface)
            delete surface;

        surface = new Surface(client, this);

        setCentralWidget(surface);

        connect(this, SIGNAL(key(Qt::Key&)), surface, SLOT(keyPress(Qt::Key&)));

        connect(surface, SIGNAL(foodEaten()), this, SLOT(updateScore()));
        connect(surface, SIGNAL(gameOver()),  this, SLOT(gameOver()));

        score = -1;

        updateScore();
    }
    else
    {
        delete client;

        client = NULL;

        QMessageBox msg;
        msg.setWindowTitle("Error");
        msg.setText("Skřítek nedokázal najít cestu na rozcestí, aby se dostal do dalšího počítače.");
        msg.setIcon(QMessageBox::Information);

        msg.exec();
    }
}

void MainWindow::showError(QString error)
{
    QMessageBox::information(this, "Error", error);
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    Qt::Key _key = (Qt::Key)event->key();

    switch(event->key())
    {
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Insert:
            emit key(_key);
            break;
    }
}
