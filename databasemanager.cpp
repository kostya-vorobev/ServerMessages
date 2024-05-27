#include "databasemanager.h"

DatabaseManager::DatabaseManager()
    : m_db(m_cm.open())
{
}

DatabaseManager::~DatabaseManager()
{
    m_cm.close(m_db);
}

void DatabaseManager::saveMessage(const QString& sender, const QString& receiver, const QString& msg, const QString& timestamp)
{
    QString tableName = composeTableName(sender, receiver);

    QSqlQuery query(m_db);

    query.prepare(QString("CREATE TABLE IF NOT EXISTS %1 (id INTEGER PRIMARY KEY AUTOINCREMENT, sender TEXT, receiver TEXT, msg TEXT, timestamp DATETIME)").arg(tableName));
    if(!query.exec()) qDebug() << "SQLException: " << query.lastError().text();

    query.prepare(QString("INSERT INTO %1 (sender, receiver, msg, timestamp) VALUES (?,?,?,?)").arg(tableName));
    query.addBindValue(sender);
    query.addBindValue(receiver);
    query.addBindValue(msg);
    query.addBindValue(timestamp);

    if(!query.exec()) qDebug() << "SQLException: " << query.lastError().text();
}

QString DatabaseManager::makeDBSafe(const QString& str) {
    QString newStr = str;
    newStr.remove(QRegExp("\\W"));
    return newStr;
}

QString DatabaseManager::composeTableName(const QString& user1, const QString& user2) {
    if (user1 < user2) {
        return makeDBSafe(user1) + "_" + makeDBSafe(user2);
    } else {
        return makeDBSafe(user2) + "_" + makeDBSafe(user1);
    }
}

QStringList DatabaseManager::getNewMessages(const QString& username, const QString& usernameAuth, const QString& lastCheckedTimeStamp)
{
    QString tableName = composeTableName(username, usernameAuth);

    QSqlQuery query(m_db);

    query.prepare(QString("SELECT sender, msg, timestamp FROM %1 WHERE timestamp > :lastChecked ORDER BY id ASC").arg(tableName));
    query.bindValue(":lastChecked", lastCheckedTimeStamp);

    if(!query.exec()) {
        qDebug() << "SQLException: " << query.lastError().text();
        return QStringList();
    }

    QStringList newMessages;

    while(query.next()) {
        QString sender = query.value(0).toString();
        QString message = query.value(1).toString();
        QString timestamp = query.value(2).toString();

        newMessages.append(sender + "|" + message + "|" + timestamp);
    }

    return newMessages;
}

QStringList DatabaseManager::getUserChats(const QString& username) {
    QSqlQuery query(m_db);

    query.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name LIKE :username;");

    query.bindValue(":username", "%" + makeDBSafe(username) + "%");

    if (!query.exec()) {
        qDebug() << "SQLException: " << query.lastError().text();
        return QStringList();
    }

    QStringList userChats;

    while(query.next()) {
        QString tableName = query.value(0).toString();
        qDebug() << tableName;

        // Extract the chat partner's name
        QString otherUser;
        QStringList users = tableName.split("_");
        for (const QString& user : users) {
            if (user != makeDBSafe(username)) {
                otherUser = user;
                break;
            }
        }

        // Get the latest message
        QSqlQuery messageQuery(m_db);
        messageQuery.prepare("SELECT msg, timestamp, sender FROM " + tableName + " ORDER BY id DESC LIMIT 1;");
        if (!messageQuery.exec()) {
            qDebug() << "SQLException: " << messageQuery.lastError().text();
            userChats.append(otherUser + "|Ошибка при получении последнего сообщения");
        } else if (messageQuery.next()) {
            QString lastMessage = messageQuery.value(2).toString() + ": " + messageQuery.value(0).toString();
            QString timestamp = messageQuery.value(1).toString();
            userChats.append(otherUser + "|" + lastMessage + "|" +timestamp);
        } else {
            userChats.append(otherUser + "|Нет сообщений");
        }
    }

    return userChats;
}
