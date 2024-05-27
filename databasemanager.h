#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <QSqlDatabase>
#include "connectionmanager.h"
#include <QSqlQuery>
#include <QDebug>

class DatabaseManager
{
public:
    DatabaseManager();
    ~DatabaseManager();

    void saveMessage(const QString& sender, const QString& receiver, const QString& msg, const QString& timestamp);
    QStringList getNewMessages(const QString& username, const QString& usernameAuth, const QString& lastCheckedTimeStamp);
    QStringList getUserChats(const QString& username);
private:
    ConnectionManager m_cm;
    QString makeDBSafe(const QString& str);
    QString composeTableName(const QString& user1, const QString& user2);
    QSqlDatabase m_db;
};

#endif // DATABASEMANAGER_H
