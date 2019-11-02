#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QDebug>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QMainWindow>
#include <QBrush>
#include <QPen>
#include <QDateTime>
#include <QTimer>

namespace Ui {
class MainWindow;
}
struct kletka{
    int bomb_count = 0;
    bool bomb = false;
    bool flag = false;
    bool open = false;
    bool red = false;
};
typedef  struct kletka Kletka;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void drawBomb(QPainter &painter,int x,int y,bool fl);
    void bombs_distribution(int i0,int j0);
    void drawKletka(QPainter &painter,int x,int y);
    void drawFlag(QPainter &painter,int x,int y);
    void drawDigit(QPainter &painter,int x,int y,int count);
    void field_open(int i,int j);
    void drawSmile1(QPainter &painter);
    void drawSmile2(QPainter &painter);
    void drawSmile3(QPainter &painter);
public slots:
    void update();
    void mode_1();
    void mode_2();
    void mode_3();
    void time_stop();
private:
    Ui::MainWindow *ui;
    bool timestop = false;
    int n = 9;
    int m = 9;
    int bombs = 10;
    int x0 = 0;
    int y0 = 75;
    bool bomb_distrd = false;
    bool lose = false;
    int game_time = 0;
    int count_bombs = bombs;
    bool first_click = false;
    int opened = 0;
    bool win = false;
    bool win_window_showed = false;
    Kletka map[16][30];
};

#endif // MAINWINDOW_H
