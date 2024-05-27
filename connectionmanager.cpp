#include "connectionmanager.h"

ConnectionManager::ConnectionManager() {}

QSqlDatabase ConnectionManager::open(){
        QString connectionName = QUuid::createUuid().toString();

        QString databasePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir dir(databasePath);
        if (!dir.exists()) { dir.mkpath("."); }

        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName(databasePath + "/Messager.db");

        if (!db.open()) {
            qDebug() << "Ошибка открытия базы данных: " << db.lastError().text();
        }
        return db;
}

void ConnectionManager::close(QSqlDatabase &db){
    QString connectionName = db.connectionName();
    db.close();
    db = QSqlDatabase();
    QSqlDatabase::removeDatabase(connectionName);
}
