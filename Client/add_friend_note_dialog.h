#ifndef ADD_FRIEND_NOTE_DIALOG_H
#define ADD_FRIEND_NOTE_DIALOG_H

#include <QDialog>

namespace Ui {
class add_friend_note_Dialog;
}

class add_friend_note_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit add_friend_note_Dialog(QWidget *parent = nullptr);
    ~add_friend_note_Dialog();

private:
    Ui::add_friend_note_Dialog *ui;
};

#endif // ADD_FRIEND_NOTE_DIALOG_H
