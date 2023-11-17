#include "movie.h"
#include "ui_movie.h"
#include<QImage>
#include<QMovie>
#include<QTimer>
#include"homedialog.h"

Movie::Movie(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Movie)
{
    ui->setupUi(this);
    this->setFixedSize(1280,655);
    ui->movie_table->setGeometry(0,0,1280,655);
    QImage image(QString("/mnt/hgfs/SharingFileVM/2.gif"));
    ui->movie_table->setPixmap(QPixmap::fromImage(image));
    QMovie *movie = new QMovie(QString("/mnt/hgfs/SharingFileVM/2.gif"));
    ui->movie_table->setMovie(movie);
    movie->start();
    QTimer *timer = new QTimer();
    timer->start(9000);
    connect(timer,&QTimer::timeout,[=](){
        timer->stop();
        this->close();
        HomeDialog *home = new HomeDialog();
        home->show();
    });

}

Movie::~Movie()
{
    delete ui;
}
