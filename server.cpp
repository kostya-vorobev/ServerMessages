#include "server.h"

// Используется для управления серверной стороной приложения

/* Конструктор класса без родительского объекта.
   Конструктор с родительским объектом предназначен для управления временем жизни сервера в зависимости от времени жизни родительского объекта. */
Server::Server()
    : QObject(nullptr)
    , m_server(nullptr)
{ }

Server::Server(QObject *parent)
    : QObject(parent)
    , m_server(nullptr)
{
}

// Деструктор, останавливает сервер и освобождает память
Server::~Server() {
    if (m_server)
    {
        m_server->close();
        delete m_server;
        m_server = nullptr;
    }
}

void Server::startServer()
{
    // Создаем новый сервер
    m_server = new QTcpServer(this);

    // Устанавливаем соединение, чтобы обрабатывать новые входящие клиентские соединения
    connect(m_server, &QTcpServer::newConnection, [&]() {
        // Принимаем следующее ожидающее соединение от клиента
        auto client = m_server->nextPendingConnection();

        // Обработка готовности к чтению нового сообщения от клиента
        connect(client, &QTcpSocket::readyRead, [=]() {
            // Разбиваем сообщение от клиента на отдельные части для обработки
            QStringList messageParts = QString::fromUtf8(client->readAll()).split("\n");
            // Получаем имя запрошенной операции
            QString operation = messageParts.value(0).trimmed();
            // Определение типа операции и ее обработка
            // Обработка регистрации нового пользователя
            // Обработка входа в систему (аутентификация)
            // Обработка поиска пользователя
            // Обработка сохранения сообщения
            // Обработка проверки новых сообщений
            // Обработка запроса на получение чатов

            if (operation == "register") {
                QString login = messageParts.value(1).trimmed();
                QString password = messageParts.value(2).trimmed();
                if (m_userManager.registerUser(login, password)) {
                    qDebug() << "Успешная регистрация: " << login;
                    client->write("Успешная регистрация");
                } else {
                    qDebug() << "Ошибка регистрации: " << login;
                    client->write("Ошибка регистрации");
                }
            }
            if (operation == "login"){
                QString login = messageParts.value(1).trimmed();
                QString password = messageParts.value(2).trimmed();
                if (m_userManager.authenticateUser(login, password)) {
                    qDebug() << "Успешная аутентификация: " << login;
                    client->write("Успешная аутентификация");
                } else {
                    qDebug() << "Ошибка аутентификации: " << login;
                    client->write("Ошибка аутентификации");
                }
            }
            if (operation == "search") {
                                QString login = messageParts.value(1).trimmed();
                SearchManager searchManager;
                QStringList searchResults = searchManager.searchUsers(messageParts.value(1), login);
                client->write(searchResults.join("\n").toUtf8());
            }
            if (operation == "saveMessage") {
                QString sender = messageParts.value(1).trimmed();
                QString receiver = messageParts.value(2).trimmed();
                QString msg = messageParts.value(3).trimmed();
                QString timestamp = messageParts.value(4).trimmed();

                // Вместо написания запросов здесь, вызываем функцию в классе DatabaseManager
                m_dbManager.saveMessage(sender, receiver, msg, timestamp);
            }

            if (operation == "checkNewMessages") {
                if (messageParts.size() < 3)
                {
                    qDebug() << "Not enough parameters for checkNewMessages.";
                    return;
                }

                QString userAuth = messageParts.value(1).trimmed();
                QString user = messageParts.value(2).trimmed();
                QString lastCheckedTimeStamp = messageParts.value(3).trimmed();


                QStringList newMessages = m_dbManager.getNewMessages(userAuth, user, lastCheckedTimeStamp);
                client->write(newMessages.join("\n").toUtf8());
            }
            if (operation == "getChats") {
                QString user = messageParts.value(1).trimmed();

                QStringList chats = m_dbManager.getUserChats(user);
                qDebug() << "Отправка чатов: " << chats;
                client->write(chats.join("\n").toUtf8());
            }


        });


        // Обрабатываем отключение клиента от сервера, удаляем клиента после его отключения
        connect(client, &QTcpSocket::disconnected, [=]() {
            if (client->state() == QAbstractSocket::UnconnectedState) {
                client->deleteLater();
            }
        });
    });

    // Пытаемся запустить сервер и прослушивать все доступные сетевые интерфейсы на выбранном порту
    if (!m_server->listen(QHostAddress::Any, 4242))  // Замените на порт, который вы хотите использовать
        qDebug("Не удалось запустить сервер");
    else
        qDebug("Сервер успешно запущен");
}


