#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QtSql>

class ConnectionManager
{
public:
    ConnectionManager();
    QSqlDatabase open();

    void close(QSqlDatabase &db);
};


#endif // CONNECTIONMANAGER_H
