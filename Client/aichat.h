#ifndef AICHAT_H
#define AICHAT_H

#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstring>
#include <QString>
using namespace std;
const int MAX_RESP = 4;

vector<string> find_match(string input);

void copy(string array[],vector<string> &v);

typedef struct{
    string input;
    string responses[MAX_RESP];
}record;

QString aichat(QString text);
#endif // AICHAT_H
