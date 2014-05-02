#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "surface.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent* event);

private:
    /// Pointers
    Ui::MainWindow* ui;

    Surface* surface;

    /// Locals
    int score;

    // Reconnect data
    QString IP;
    quint16 port;

private slots:
    void gameOver();
    void newGame();

    void updateScore();

    void validateConnection();
    void rejoin();

    void showError(QString error);

signals:
    void key(Qt::Key& key);
};

#endif // MAINWINDOW_H