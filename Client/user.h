#ifndef USER_H
#define USER_H
#include<QString>

class user
{
public:
    QString name;
    QString note_name;
    QString password;
    bool is_online;
    QString ip;
    int port;
    QString flag_head="0";
    QString flag_theme="0";


    user(){
        is_online=false;
        ip = "192.168.154.131";
        port = 9000;
    }
};

#endif // USER_H
