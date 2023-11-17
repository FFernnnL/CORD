#include "add_friend_note_dialog.h"
#include "ui_add_friend_note_dialog.h"

add_friend_note_Dialog::add_friend_note_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::add_friend_note_Dialog)
{
    ui->setupUi(this);
}

add_friend_note_Dialog::~add_friend_note_Dialog()
{
    delete ui;
}
