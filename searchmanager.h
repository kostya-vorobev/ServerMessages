#ifndef SEARCHMANAGER_H
#define SEARCHMANAGER_H

#include <QObject>
#include <QDebug>
#include <QtSql/QSqlQuery>
#include "connectionmanager.h"

class SearchManager: public QObject {
    Q_OBJECT

public:
    SearchManager(QObject* parent = nullptr);
    QStringList searchUsers(const QString& searchText, const QString& currentUsername);

private:
    QStringList reviewChats(const QString& currentUsername);
};

#endif // SEARCHMANAGER_H
