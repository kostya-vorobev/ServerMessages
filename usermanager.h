#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QCoreApplication>
#include <QtSql/QSqlError>
#include <QDir>
#include <QStandardPaths>
#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>


class UserManager {
public:
    UserManager();
    bool registerUser(const QString& username, const QString& password);
    bool authenticateUser(const QString& username, const QString& password);
    // Дополнительные функции, если они необходимы
};

#endif // USERMANAGER_H
