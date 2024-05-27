#include "searchmanager.h"

SearchManager::SearchManager(QObject* parent)
    : QObject(parent) { }

QStringList SearchManager::searchUsers(const QString& searchText, const QString& currentUsername) {
    QStringList users;
    ConnectionManager connectionManager;
    QSqlDatabase db = connectionManager.open();

    QSqlQuery query(db);
    query.prepare("SELECT login FROM user_auth WHERE login LIKE :searchText AND login <> :currentUsername");
    query.bindValue(":searchText", "%" + searchText + "%");
    query.bindValue(":currentUsername", currentUsername);
    query.exec();
    while (query.next())
    {
        users << query.value(0).toString();
    }

    connectionManager.close(db);

    QStringList reviewedUsers = reviewChats(currentUsername);

    QStringList prioritizedUsers;
    for (const QString &user : reviewedUsers)
    {
        if (users.removeOne(user))
        {
            prioritizedUsers << user;
        }
    }

    prioritizedUsers << users;

    return prioritizedUsers;
}

QStringList SearchManager::reviewChats(const QString& currentUsername) {
    QStringList reviewedUsers;
    ConnectionManager connectionManager;
    QSqlDatabase db = connectionManager.open();

    QSqlQuery query(db);
    // Здесь пропишите SQL запрос к вашей базе данных
    // чтобы получить список пользователей, с которыми был обмен сообщениями

    connectionManager.close(db);
    return reviewedUsers;
}
