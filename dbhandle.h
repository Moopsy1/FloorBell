#ifndef DBHANDLE_H
#define DBHANDLE_H
#include <QString>

class DBhandle
{
private:
    void createTable();
public:
    DBhandle();
    void DBadd(QString text);
};

#endif // DBHANDLE_H
