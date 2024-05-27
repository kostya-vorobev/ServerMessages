#include "connectionmanager.h"

// Конструктор класса
ConnectionManager::ConnectionManager() {}

// Функция открытия подключения к базе данных
QSqlDatabase ConnectionManager::open(){
    // Создаем уникальное имя для подключения
    QString connectionName = QUuid::createUuid().toString();

    // Получаем путь к приложению
    QString databasePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    // Если папки не существует, то создаем ее
    QDir dir(databasePath);
    if (!dir.exists()) { dir.mkpath("."); }

    // Открытие подключения к Базе Данных
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName(databasePath + "/Messager.db");

    // Проверка успешности открытия подключения
    if (!db.open()) {
        qDebug() << "Ошибка открытия базы данных: " << db.lastError().text();
    }
    // Возвращение подключения к БД
    return db;
}

// Функция закрытия подключения к базе данных
void ConnectionManager::close(QSqlDatabase &db){
    // Закрываем БД и удаляем подключение
    QString connectionName = db.connectionName();
    db.close();
    db = QSqlDatabase();
    QSqlDatabase::removeDatabase(connectionName);
}
