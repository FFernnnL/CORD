#ifndef MOVIE_H
#define MOVIE_H

#include <QDialog>

namespace Ui {
class Movie;
}

class Movie : public QDialog
{
    Q_OBJECT

public:
    explicit Movie(QWidget *parent = nullptr);
    ~Movie();

private:
    Ui::Movie *ui;
};

#endif // MOVIE_H
