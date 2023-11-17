#ifndef SENDDIALOG_H
#define SENDDIALOG_H

#include <QDialog>

namespace Ui {
class SendDialog;
}

class SendDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SendDialog(QWidget *parent = nullptr);
    ~SendDialog();

private slots:
    void on_choseBtn_clicked();

    void on_sendfileBtn_clicked();

    void on_sendprogressBar_valueChanged();

private:
    Ui::SendDialog *ui;
};

#endif // SENDDIALOG_H
