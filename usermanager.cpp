#include "usermanager.h"
#include "connectionmanager.h"
#include <QCryptographicHash>

UserManager::UserManager() {

}

bool UserManager::registerUser(const QString& username, const QString& password) {
    ConnectionManager cm;
    QSqlDatabase db = cm.open();

    QString hashedPassword = QCryptographicHash::hash((password + username).toUtf8(), QCryptographicHash::Sha256).toHex();

    QSqlQuery query(db);
    query.prepare("INSERT INTO user_auth (login, password) VALUES (?, ?)");
    query.addBindValue(username);
    query.addBindValue(hashedPassword);

    if (!query.exec()) {
        qDebug() << "Ошибка регистрации пользователя: " << query.lastError().text();
        cm.close(db);
        return false;
    }

    cm.close(db);
    return true;
}

bool UserManager::authenticateUser(const QString& username, const QString& password) {
    ConnectionManager cm;
    QSqlDatabase db = cm.open();

    QString hashedPassword = QCryptographicHash::hash((password + username).toUtf8(), QCryptographicHash::Sha256).toHex();

    QSqlQuery query(db);
    query.prepare("SELECT password FROM user_auth WHERE login = ?");
    query.addBindValue(username);

    if (query.exec() && query.next() && query.value(0).toString() == hashedPassword){
        cm.close(db);
        return true;
    }
    else {
        qDebug() << "Ошибка аутентификации: " << query.lastError().text() << username << "\n" << query.value(0).toString() << "\n" <<hashedPassword;
    }

    cm.close(db);
    return false;
}
