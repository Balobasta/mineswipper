#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTime>
#include <QTimer>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QTimer *timer=new QTimer;
    connect(timer,SIGNAL(timeout()),this,SLOT(update()));
    connect(ui->novichok,SIGNAL(triggered()),this,SLOT(mode_1()));
    connect(ui->lubitel,SIGNAL(triggered()),this,SLOT(mode_2()));
    connect(ui->profi,SIGNAL(triggered()),this,SLOT(mode_3()));
    connect(ui->pause,SIGNAL(triggered()),this,SLOT(time_stop()));
    timer->start(1000);
    ui->centralWidget->setFixedSize(250,240);
    mode_1();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    int x_smile = ((ui->Bombas->geometry().x()+ui->Bombas->geometry().width())+(ui->Time->geometry().x()))/2 -20;
    int y_smile = ui->Bombas->geometry().y()+26;
    if (x_smile && y_smile && event->x()>=x_smile && event->x()<=x_smile + 40 && event->y()>=y_smile && event->y()<=y_smile + 40){
        bomb_distrd = false;
        lose = false;
        game_time = 0;
        count_bombs = bombs;
        first_click = false;
        opened = 0;
        win = false;
        win_window_showed = false;
        for(int i = 0;i<n;i++){
            for(int j = 0;j<m;j++){
                map[i][j].bomb_count = 0;
                map[i][j].bomb = false;
                map[i][j].flag = false;
                map[i][j].open = false;
                map[i][j].red = false;
            }
        }
        repaint();
        return;
    }
    if (lose) return;
    if(win) return;
    if (event->x()>=x0 && event->x()<=x0+20*m && event->y()>=y0 && event->y()<=y0+20*n){
        if (!first_click){
            first_click = true;
            int i_pos = static_cast<int>(floor((event->y()-y0)/20));
            int j_pos = static_cast<int>(floor((event->x()-x0)/20));
            bombs_distribution(i_pos,j_pos);
        }
    }else return;
    if(timestop) timestop = false;
    int j = static_cast<int>(floor((event->x()-x0)/20));
    int i = static_cast<int>(floor((event->y()-y0)/20));
    if(event->button() == Qt::LeftButton){
        if (map[i][j].flag) return;
        if(!map[i][j].open){
            if(map[i][j].bomb){
            map[i][j].red = true;
            lose  = true;
            first_click = false;
            map[i][j].open = true;
            }else field_open(i,j);
        }
    }else if(event->button() == Qt::RightButton){
        if (map[i][j].open) return;
        if (!map[i][j].flag ){
            map[i][j].flag = true;
            count_bombs--;
            ui->Bombas->display(QString::number(count_bombs,10));
        }
        else{
            map[i][j].flag = false;
            count_bombs++;
            ui->Bombas->display(QString::number(count_bombs,10));
        }
    }
    if (opened == n*m - bombs ){
        if(!win_window_showed){
            first_click = false;
            win = true;
        }
    }
    repaint();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    x0 = (this->width()-(20*m))/2;
    y0 = 75;
    QPainter painter(this);
    QPen pen;
    pen.setWidth(1);
    painter.setPen(pen);
    //раскраска виджета
    painter.fillRect(0,0,this->width(),this->height(),Qt::gray);
    QColor color(200,211,210);
    pen.setColor(color);
    pen.setWidth(3);
    painter.setPen(pen);
    painter.drawLine(x0-2,73,x0-2,this->geometry().height()-1);
    painter.drawLine(1,this->geometry().height()-2,this->geometry().width()-2,this->geometry().height()-2);
    painter.drawLine(x0-1,73,x0+20*m+2,73);
    painter.drawLine(this->geometry().width()-2,68,this->geometry().width()-2,this->geometry().height()-2);
    painter.drawLine(x0+20*m+2,this->geometry().height()-2,this->geometry().width()-2,this->geometry().height()-2);

    color.setRgb(100,100,100);
    pen.setColor(color);
    painter.setPen(pen);
    painter.drawLine(1,68,this->geometry().width()-2,68);
    painter.drawLine(1,68,1,this->geometry().height()-2);
    painter.drawLine(x0+20*m+2,76,x0+20*m+2,75+20*n-1);
    color.setRgb(255,255,210);
    painter.fillRect(x0,y0,20*m,20*n,Qt::gray);
    pen.setWidth(1);
    pen.setColor(Qt::black);
    painter.setPen(pen);
    QBrush k(Qt::black);
    painter.setBrush(k);
    for(int i = x0;i<=x0+20*m;i+=20)
        painter.drawLine(i,y0,i,y0+20*n);
    for(int j = y0;j<=y0+20*n;j+=20)
        painter.drawLine(x0,j,x0+20*m,j);
    //распределение бомб
    if(win){
        for(int i = 0; i < n; i++)
            for(int j = 0; j < m; j++)
                if (map[i][j].bomb){
                    drawFlag(painter,x0+20*j,y0+20*i);
                }else if(map[i][j].open)
                    drawDigit(painter,x0+20*j,y0+20*i,map[i][j].bomb_count);

    }else
    if(lose){
        for(int i = 0; i < n; i++)//   y
            for(int j = 0; j < m; j++)/*  x  */{
                if (map[i][j].bomb){
                    bool ok;
                    if (map[i][j].flag) ok = true;
                    else ok = false;
                    drawBomb(painter,x0+20*j,y0+20*i,ok);
                }else if(map[i][j].open){
                    drawDigit(painter,x0+20*j,y0+20*i,map[i][j].bomb_count);
                }else if(map[i][j].flag){
                    drawFlag(painter,x0+20*j,y0+20*i);
                }else drawKletka(painter,x0+20*j,y0+20*i);
            }
    }else
    for(int i = 0; i < n ; i++)
        for(int j = 0; j < m ; j ++){
            if(map[i][j].open){
                drawDigit(painter,x0+20*j,y0+20*i,map[i][j].bomb_count);
            }else{
                if(map[i][j].flag) drawFlag(painter,x0+20*j,y0+20*i);
                else
                    drawKletka(painter,x0+20*j,y0+20*i);
            }
        }
    if(win){
        drawSmile3(painter);
    }else if(lose){
        drawSmile2(painter);
    }else drawSmile1(painter);
}

void MainWindow::drawBomb(QPainter &painter,int x,int y,bool fl)
{
    QPen p;
    QBrush k(Qt::red);
    p.setWidth(1);
    painter.setPen(p);

//красный прямоугольник
    painter.setBrush(k);
    int j = static_cast<int>(floor((x-x0)/20));
    int i = static_cast<int>(floor((y-y0)/20));
    if (map[i][j].red){
        painter.drawRect(x,y,20,20);
    }
//черная окружность(тело бомбы)
    if (lose && fl){
        QColor c(120,120,120);
        k.setColor(c);
        painter.setBrush(k);
    }else{
        k.setColor(Qt::black);
        painter.setBrush(k);
    }
    painter.drawEllipse(x+4,y+4,12,12);

//линии
    if (lose && fl){
        QColor c(120,120,120);
        k.setColor(c);
        painter.setBrush(k);
        p.setColor(c);
        painter.setPen(p);
    }else{
        k.setColor(Qt::black);
        painter.setBrush(k);
    }
    p.setWidth(2);
    painter.setPen(p);
    painter.drawLine(x+5,y+5,x+16,y+16);
    painter.drawLine(x+5,y+16,x+16,y+5);
    painter.drawLine(x+10,y+3,x+10,y+18);
    painter.drawLine(x+3,y+10,x+18,y+10);
//белая окружность(отблик)
    p.setWidth(1);
    painter.setPen(p);
    k.setColor(Qt::white);
    painter.setBrush(k);
    painter.drawEllipse(x+7,y+7,3,3);
    if (lose && fl){
        QPen pen;
        pen.setColor(Qt::black);
        pen.setWidth(3);
        painter.setPen(pen);
        painter.drawLine(x+10,y+5,x+10,y+14);
        painter.drawLine(x+7,y+14,x+13,y+14);
        pen.setWidth(2);
        pen.setColor(Qt::red);
        painter.setPen(pen);
        x = x+2;
        for(int h = y+5; h <= y+8 ;h++)
            painter.drawLine(x+10,h,(h+x-y+5),h);
        for(int h = y+9; h <=y+11; h++){
            painter.drawLine(x+10,h,(x+y-h+21),h);
        }
    }
}

void MainWindow::bombs_distribution(int i0,int j0)
{
    if (bomb_distrd) return;
    QTime midnight(0,0,0);
    qsrand(static_cast<uint>(midnight.secsTo(QTime::currentTime())));
    int k = 0;
    while(k!=bombs){
        int i = qrand()%n;
        int j = qrand()%m;
        if (!map[i][j].bomb && (i != i0 && j != j0)){
            map[i][j].bomb = true;
            k++;
        }
    }
    //подсчет бомб в окрестности ячейки
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            if (map[i][j].bomb) continue;
            int count = 0;
            if (i-1 >= 0){
                if (j-1>=0){
                    if(map[i-1][j-1].bomb) count++;
                }
                if (map[i-1][j].bomb) count++;
                if(j+1<m) if(map[i-1][j+1].bomb) count++;
            }
            if (j-1>=0){
                if (map[i][j-1].bomb) count++;
                if (i+1 < n) if (map[i+1][j-1].bomb) count++;
            }
            if (i+1 < n) if (map[i+1][j].bomb) count++;
            if (j + 1 < m){
                if (map[i][j+1].bomb) count++;
                if (i+1<n) if (map[i+1][j+1].bomb) count++;
            }
            map[i][j].bomb_count = count;
        }
    }
    bomb_distrd = true;
}

void MainWindow::drawFlag(QPainter &painter, int x, int y)
{
    QColor color(200,211,210);
    QPen p;
    QBrush k(Qt::black);
    painter.setBrush(k);
    p.setColor(color);
    p.setWidth(3);
    painter.setPen(p);
    painter.drawLine(x+2,y+2,x+18,y+2);
    painter.drawLine(x+2,y+2,x+2,y+18);
    color.setRgb(100,100,100);
    p.setColor(color);
    painter.setPen(p);
    painter.drawLine(x+18,y+2,x+18,y+18);
    painter.drawLine(x+2,y+18,x+18,y+18);

    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(3);
    painter.setPen(pen);
    painter.drawLine(x+10,y+5,x+10,y+14);
    painter.drawLine(x+7,y+14,x+13,y+14);
    pen.setWidth(2);
    pen.setColor(Qt::red);
    painter.setPen(pen);
    x = x+2;
    for(int h = y+5; h <= y+8 ;h++)
        painter.drawLine(x+10,h,(h+x-y+5),h);
    for(int h = y+9; h <=y+11; h++){
        painter.drawLine(x+10,h,(x+y-h+21),h);
    }
}

void MainWindow::drawDigit(QPainter &painter, int x, int y,int count)
{
    QBrush k (Qt::gray);
    QPen pen;
    pen.setWidth(1);
    painter.setPen(pen);
    painter.setBrush(k);
    painter.drawRect(x,y,20,20);
    QFont font;
    font.setBold(true);

    font.setPixelSize(18);
    painter.setFont(font);
    switch (count) {
    case 1:{
        QColor c(0,13,189);
        pen.setColor(c);
        painter.setPen(pen);
        painter.drawText(x+4,y+17,"1");
        break;
    }
    case 2:{
        QColor c(0,110,0);
        pen.setColor(c);
        painter.setPen(pen);
        painter.drawText(x+4,y+17,"2");
        break;
    }
    case 3:{
        QColor c(186,0,0);
        pen.setColor(Qt::red);
        painter.setPen(pen);
        painter.drawText(x+4,y+17,"3");
        break;
    }
    case 4:{
        QColor c(0,0,220);
        pen.setColor(c);
        painter.setPen(pen);
        painter.drawText(x+4,y+17,"4");
        break;
    }
    case 5:{
        QColor c(107,0,0);
        pen.setColor(c);
        painter.setPen(pen);
        painter.drawText(x+4,y+17,"5");
        break;
    }
    case 6:{
        QColor c(0,137,206);
        pen.setColor(c);
        painter.setPen(pen);
        painter.drawText(x+4,y+17,"6");
        break;
    }
    case 7:{
        QColor c(113,0,0);
        pen.setColor(c);
        painter.setPen(pen);
        painter.drawText(x+4,y+17,"7");
        break;
    }
    case 8:{
        QColor c(64,65,71);
        pen.setColor(c);
        painter.setPen(pen);
        painter.drawText(x+4,y+17,"8");
        break;
    }
    }
}

void MainWindow::field_open(int i, int j)
{
    if(map[i][j].flag || map[i][j].bomb) return;
    if (!map[i][j].open){
        map[i][j].open = true;
        opened++;
    }
    if (opened == n*m - bombs){
        if(!win_window_showed){
            win_window_showed = true;
            first_click = false;
            win = true;
            count_bombs = 0;
        }
    }
    int t = 0;
    if(map[i][j].bomb_count){
        for(int k = i-1; k <= i+1; k++){
            if (k < 0 || k >= n) continue;
            for(int h = j-1; h <= j+1; h++){
                if (h < 0 || h >= m || (k == i && h == j)) continue;
                if(map[k][h].bomb && map[k][h].flag){
                    t++;
                }
            }
        }
        if (t != map[i][j].bomb_count)
        return;
    }
    if(t==map[i][j].bomb_count && t!=0){
        for(int k = i-1; k <= i+1; k++){
            if (k < 0 || k >= n) continue;
            for(int h = j-1; h <= j+1; h++){
                if (h < 0 || h >= m || (k == i && h == j)) continue;
                if(!map[k][h].bomb && !map[k][h].flag){
                    if(!map[k][h].bomb_count) field_open(k,h);
                        else{
                        if(!map[k][h].open){
                            map[k][h].open = true;
                            opened++;
                        }
                            if (opened == n*m - bombs){
                                if(!win_window_showed){
                                    win_window_showed = true;
                                    QMessageBox *box = new QMessageBox();
                                    box->setText("!!!You are win!!!");
                                    box->show();
                                    first_click = false;
                                    win = true;
                                    count_bombs = 0;
                                }
                            }
                    }
                }
            }
        }
    }else
    for(int k = i-1; k <= i+1; k++){
        if (k < 0 || k >= n) continue;
        for(int h = j-1; h <= j+1; h++){
            if (h < 0 || h >= m || (k == i && h == j)) continue;
            if(!map[k][h].bomb || !map[k][h].flag){
                if(map[k][h].open) continue;
                field_open(k,h);
            }
        }
    }
}

void MainWindow::drawSmile1(QPainter &painter)
{
    //обычный смайлик
    int x = ((ui->Bombas->geometry().x()+ui->Bombas->geometry().width())+(ui->Time->geometry().x()))/2 -20;
    int y = ui->Bombas->geometry().y()+26;
    QBrush k(Qt::gray);
    QPen pen;
    pen.setWidth(1);
    pen.setColor(Qt::black);
    painter.setPen(pen);
    painter.setBrush(k);
    painter.drawRect(x,y,40,40);

    QColor color(200,211,210);
    pen.setColor(color);
    pen.setWidth(3);
    painter.setPen(pen);
    painter.drawLine(x,y,x+40,y);
    painter.drawLine(x,y,x,y+40);

    color.setRgb(100,100,100);
    pen.setColor(color);
    painter.setPen(pen);
    painter.drawLine(x,y+40,x+40,y+40);
    painter.drawLine(x+40,y+40,x+40,y);

    k.setColor(Qt::yellow);
    painter.setBrush(k);
    pen.setWidth(1);
    painter.setPen(pen);
    painter.drawEllipse(x+5,y+5,30,30);
    pen.setWidth(3);
    pen.setColor(Qt::black);
    painter.setPen(pen);
    painter.drawPoint(x+15,y+17);
    painter.drawPoint(x+25,y+17);

    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawArc(x+13,y+15,15,15,20,-3000);
}

void MainWindow::drawSmile2(QPainter &painter)
{
    //смайлик во время проигрыша
    int x = ((ui->Bombas->geometry().x()+ui->Bombas->geometry().width())+(ui->Time->geometry().x()))/2 -20;
    int y = ui->Bombas->geometry().y()+26;
    QBrush k(Qt::gray);
    QPen pen;
    pen.setWidth(1);
    pen.setColor(Qt::black);
    painter.setPen(pen);
    painter.setBrush(k);
    painter.drawRect(x,y,40,40);

    QColor color(200,211,210);
    pen.setColor(color);
    pen.setWidth(3);
    painter.setPen(pen);
    painter.drawLine(x,y,x+40,y);
    painter.drawLine(x,y,x,y+40);

    color.setRgb(100,100,100);
    pen.setColor(color);
    painter.setPen(pen);
    painter.drawLine(x,y+40,x+40,y+40);
    painter.drawLine(x+40,y+40,x+40,y);

    k.setColor(Qt::yellow);
    painter.setBrush(k);
    pen.setWidth(1);
    painter.setPen(pen);
    painter.drawEllipse(x+5,y+5,30,30);
    pen.setWidth(3);
    pen.setColor(Qt::black);
    painter.setPen(pen);
    painter.drawPoint(x+15,y+17);
    painter.drawPoint(x+25,y+17);

    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawArc(x+13,y+25,15,5,20,3000);
}

void MainWindow::drawSmile3(QPainter &painter)
{
    //смайлик победителя
    int x = ((ui->Bombas->geometry().x()+ui->Bombas->geometry().width())+(ui->Time->geometry().x()))/2 -20;
    int y = ui->Bombas->geometry().y()+26;
    QBrush k(Qt::gray);
    QPen pen;
    pen.setWidth(1);
    pen.setColor(Qt::black);
    painter.setPen(pen);
    painter.setBrush(k);
    painter.drawRect(x,y,40,40);

    QColor color(200,211,210);
    pen.setColor(color);
    pen.setWidth(3);
    painter.setPen(pen);
    painter.drawLine(x,y,x+40,y);
    painter.drawLine(x,y,x,y+40);

    color.setRgb(100,100,100);
    pen.setColor(color);
    painter.setPen(pen);
    painter.drawLine(x,y+40,x+40,y+40);
    painter.drawLine(x+40,y+40,x+40,y);

    k.setColor(Qt::yellow);
    painter.setBrush(k);
    pen.setWidth(1);
    painter.setPen(pen);
    painter.drawEllipse(x+5,y+5,30,30);
    pen.setWidth(3);
    pen.setColor(Qt::black);
    painter.setPen(pen);
    painter.drawPoint(x+15,y+17);
    painter.drawPoint(x+25,y+17);

    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawArc(x+13,y+15,15,15,20,-3000);

    pen.setColor(Qt::black);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawLine(x+11,y+16,x+30,y+16);
    painter.drawLine(x+12,y+17,x+20,y+17);
    painter.drawLine(x+13,y+18,x+19,y+18);
    painter.drawLine(x+14,y+19,x+17,y+19);
    painter.drawLine(x+21,y+17,x+29,y+17);
    painter.drawLine(x+22,y+18,x+28,y+18);
    painter.drawLine(x+23,y+19,x+26,y+19);
}

void MainWindow::drawKletka(QPainter &painter, int x, int y)
{
    QColor color(200,211,210);
    QPen p;
    QBrush k(Qt::black);
    painter.setBrush(k);
    p.setColor(color);
    p.setWidth(3);
    painter.setPen(p);
    painter.drawLine(x+2,y+2,x+18,y+2);
    painter.drawLine(x+2,y+2,x+2,y+18);
    color.setRgb(100,100,100);
    p.setColor(color);
    painter.setPen(p);
    painter.drawLine(x+18,y+2,x+18,y+18);
    painter.drawLine(x+2,y+18,x+18,y+18);
}

void MainWindow::update(){
    if (first_click && !timestop)
        game_time ++;
    ui->Time->display(QString::number(game_time,10));
    ui->Bombas->display(QString::number(count_bombs,10));
}

void MainWindow::mode_1()
{
     n = 9;
     m = 9;
     bombs = 10;
     timestop = false;
     x0 = 0;
     y0 = 0;
     bomb_distrd = false;
     lose = false;
     game_time = 0;
     count_bombs = bombs;
     first_click = false;
     opened = 0;
     win = false;
     win_window_showed = false;
     this->resize(250,260);
     this->setFixedSize(250, 260);
     for(int i = 0;i<n;i++){
         for(int j = 0;j<m;j++){
             map[i][j].bomb_count = 0;
             map[i][j].bomb = false;
             map[i][j].flag = false;
             map[i][j].open = false;
             map[i][j].red = false;
         }
     }
     repaint();
}

void MainWindow::mode_2()
{
    n = 16;
    m = 16;
    bombs = 40;
    timestop = false;
    x0 = 0;
    y0 = 0;
    bomb_distrd = false;
    lose = false;
    game_time = 0;
    count_bombs = bombs;
    first_click = false;
    opened = 0;
    win = false;
    win_window_showed = false;
    this->resize(340,400);
    this->setFixedSize(340,400);
    for(int i = 0;i<n;i++){
        for(int j = 0;j<m;j++){
            map[i][j].bomb_count = 0;
            map[i][j].bomb = false;
            map[i][j].flag = false;
            map[i][j].open = false;
            map[i][j].red = false;
        }
    }
    repaint();
}

void MainWindow::mode_3()
{
    n = 16;
    m = 30;
    bombs = 99;
    timestop = false;
    x0 = 0;
    y0 = 0;
    bomb_distrd = false;
    lose = false;
    game_time = 0;
    count_bombs = bombs;
    first_click = false;
    opened = 0;
    win = false;
    win_window_showed = false;
    this->resize(620,400);
    this->setFixedSize(660,380);
    for(int i = 0;i<n; i++){
        for(int j = 0;j<m;j++){
            map[i][j].bomb_count = 0;
            map[i][j].bomb = false;
            map[i][j].flag = false;
            map[i][j].open = false;
            map[i][j].red = false;

        }
    }
    repaint();
}

void MainWindow::time_stop()
{
    if (timestop) timestop = false;
    else timestop = true;
}




