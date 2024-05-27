#include "databasemanager.h"

// Конструктор класса, открывает базу данных
DatabaseManager::DatabaseManager()
    : m_db(m_cm.open())
{
}

// Деструктор класса, закрывает базу данных
DatabaseManager::~DatabaseManager()
{
    m_cm.close(m_db);
}

// Метод сохранения сообщения в базу данных
void DatabaseManager::saveMessage(const QString& sender, const QString& receiver, const QString& msg, const QString& timestamp)
{
    // Создание имени таблицы
    QString tableName = composeTableName(sender, receiver);
    QSqlQuery query(m_db);

    // Подгтовка запроса для создания новой таблицы, если она еще не существует
    query.prepare(QString("CREATE TABLE IF NOT EXISTS %1 (id INTEGER PRIMARY KEY AUTOINCREMENT, sender TEXT, receiver TEXT, msg TEXT, timestamp DATETIME)").arg(tableName));

    if(!query.exec()) qDebug() << "SQLException: " << query.lastError().text();

    // Подготовка запроса для добавления сообщения в таблицу
    query.prepare(QString("INSERT INTO %1 (sender, receiver, msg, timestamp) VALUES (?,?,?,?)").arg(tableName));
    // Добавление параметров к запросу
    query.addBindValue(sender);
    query.addBindValue(receiver);
    query.addBindValue(msg);
    query.addBindValue(timestamp);

    // Выполнение запроса и обработка ошибок
    if(!query.exec()) qDebug() << "SQLException: " << query.lastError().text();
}

// Метод для очистки строки от небезопасных символов
QString DatabaseManager::makeDBSafe(const QString& str) {
    // Удаление всех небуквенно-цифровых символов
    QString newStr = str;
    newStr.remove(QRegExp("\\W"));
    return newStr;
}

// Метод для создания имени таблицы из имен двух пользователей
QString DatabaseManager::composeTableName(const QString& user1, const QString& user2) {
    // Сортировка пользователей и создание имени таблицы
    if (user1 < user2) {
        return makeDBSafe(user1) + "_" + makeDBSafe(user2);
    } else {
        return makeDBSafe(user2) + "_" + makeDBSafe(user1);
    }
}

// Метод возвращает новые сообщения переданных пользователей
QStringList DatabaseManager::getNewMessages(const QString& username, const QString& usernameAuth, const QString& lastCheckedTimeStamp)
{
    // Создаем имя таблицы по именам пользователей
    QString tableName = composeTableName(username, usernameAuth);
    QSqlQuery query(m_db);

    // Подготавливаем SQL-запрос на получение новых сообщений
    query.prepare(QString("SELECT sender, msg, timestamp FROM %1 WHERE timestamp > :lastChecked ORDER BY id ASC").arg(tableName));
    query.bindValue(":lastChecked", lastCheckedTimeStamp);

    if(!query.exec()) {
        qDebug() << "SQLException: " << query.lastError().text();
        return QStringList();
    }

    QStringList newMessages;

    // Проходим по записям в результате запроса и добавляем их в список
    while(query.next()) {
        QString sender = query.value(0).toString();
        QString message = query.value(1).toString();
        QString timestamp = query.value(2).toString();

        newMessages.append(sender + "|" + message + "|" + timestamp);
    }

    return newMessages;
}

// Метод получает список чатов указанного пользователя.
QStringList DatabaseManager::getUserChats(const QString& username) {
    QSqlQuery query(m_db);

    // Подготавливаем SQL-запрос к базе данных, чтобы получить все таблицы, которые содержат имя указанного пользователя.
    query.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name LIKE :username;");

    query.bindValue(":username", "%" + makeDBSafe(username) + "%");

    if (!query.exec()) {
        qDebug() << "SQLException: " << query.lastError().text();
        return QStringList();
    }

    QStringList userChats;

    // Перебираем все полученные таблицы.
    while(query.next()) {
        // Получаем имя таблицы из результата запроса.
        QString tableName = query.value(0).toString();
        qDebug() << tableName;

        // Извлекаем имя партнера по чату из имени таблицы.
        QString otherUser;
        QStringList users = tableName.split("_");
        for (const QString& user : users) {
            if (user != makeDBSafe(username)) {
                otherUser = user;
                break;
            }
        }

        // Подготавливаем SQL-запрос для получения последнего сообщения из чата.
        QSqlQuery messageQuery(m_db);
        messageQuery.prepare("SELECT msg, timestamp, sender FROM " + tableName + " ORDER BY id DESC LIMIT 1;");

        if (!messageQuery.exec()) {
            qDebug() << "SQLException: " << messageQuery.lastError().text();
            userChats.append(otherUser + "|Ошибка при получении последнего сообщения");
        } else if (messageQuery.next()) {
            // Если сообщение найдено, добавляем его в список чатов.
            QString lastMessage = messageQuery.value(2).toString() + ": " + messageQuery.value(0).toString();
            QString timestamp = messageQuery.value(1).toString();
            userChats.append(otherUser + "|" + lastMessage + "|" +timestamp);
        } else {
            // Если сообщений нет, добавляем информацию об этом в список.
            userChats.append(otherUser + "|Нет сообщений");
        }
    }

    // Возвращаем полученный список чатов.
    return userChats;
}
