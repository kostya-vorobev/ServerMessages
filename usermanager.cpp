#include "usermanager.h"
#include "connectionmanager.h"
#include <QCryptographicHash>

// Конструктор класса UserManager
UserManager::UserManager() {
}

// Регистрация пользователя с использованием зашифрованного пароля
bool UserManager::registerUser(const QString& username, const QString& password) {
    ConnectionManager cm;
    QSqlDatabase db = cm.open();

    // Хэширование пароля с учетом имени пользователя для повышения безопасности
    QString hashedPassword = QCryptographicHash::hash(
                                 (password + username).toUtf8(),
                                 QCryptographicHash::Sha256).toHex();

    // Выполняемый SQL-запрос для регистрации пользователя
    QSqlQuery query(db);
    query.prepare("INSERT INTO user_auth (login, password) VALUES (?, ?)");
    query.addBindValue(username);
    query.addBindValue(hashedPassword);

    // Выполнение запроса и проверка результата
    if (!query.exec()) {
        qDebug() << "Ошибка регистрации пользователя: "
                 << query.lastError().text();
        cm.close(db);
        return false;
    }

    cm.close(db);
    return true;
}

// Аутентификация пользователя с паролем
bool UserManager::authenticateUser(const QString& username, const QString& password) {
    ConnectionManager cm;
    QSqlDatabase db = cm.open();

    // Генерация хэша для проверки пароля
    QString hashedPassword = QCryptographicHash::hash(
                                 (password + username).toUtf8(),
                                 QCryptographicHash::Sha256).toHex();

    // Запрос для проверки имени пользователя и пароля
    QSqlQuery query(db);
    query.prepare("SELECT password FROM user_auth WHERE login = ?");
    query.addBindValue(username);

    // Выполнение запроса и проверка успешности аутентификации
    if (query.exec() && query.next() && query.value(0).toString() == hashedPassword){
        cm.close(db);
        return true;
    }
    else {
        qDebug() << "Ошибка аутентификации: "
                 << query.lastError().text() << username
                 << "\n" << query.value(0).toString()
                 << "\n" << hashedPassword;
    }

    cm.close(db);
    return false;
}
