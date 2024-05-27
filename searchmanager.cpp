#include "searchmanager.h"

// Конструктор класса
SearchManager::SearchManager(QObject* parent)
    : QObject(parent) { }

// Метод осуществляет поиск пользователей в системе, исключая текущего пользователя
QStringList SearchManager::searchUsers(const QString& searchText, const QString& currentUsername) {
    // Создаем список для хранения найденных пользователей
    QStringList users;
    // Инициируем менеджер соединения
    ConnectionManager connectionManager;
    // Открываем соединение с базой данных
    QSqlDatabase db = connectionManager.open();

    // Создаем SQL-запрос для поиска пользователей
    QSqlQuery query(db);
    query.prepare("SELECT login FROM user_auth WHERE login LIKE :searchText AND login <> :currentUsername");
    query.bindValue(":searchText", "%" + searchText + "%");
    query.bindValue(":currentUsername", currentUsername);
    query.exec();

    // Добавляем найденных пользователей в список
    while (query.next())
    {
        users << query.value(0).toString();
    }

    // Закрываем соединение с базой данных
    connectionManager.close(db);

    // Отзываем чаты с текущим пользователем
    QStringList reviewedUsers = reviewChats(currentUsername);

    QStringList prioritizedUsers;
    // Проверяем, есть ли отозванные пользователи в списке найденных пользователей
    for (const QString &user : reviewedUsers)
    {
        if (users.removeOne(user)) // Если да, приоритет добавления в список
        {
            prioritizedUsers << user;
        }
    }

    // Получаем финальный список пользователей
    prioritizedUsers << users;

    return prioritizedUsers;
}

// Метод для отзыва чатов с текущим пользователем
QStringList SearchManager::reviewChats(const QString& currentUsername) {
    QStringList reviewedUsers;
    ConnectionManager connectionManager;
    QSqlDatabase db = connectionManager.open();

    QSqlQuery query(db);

    connectionManager.close(db);
    return reviewedUsers;
}
