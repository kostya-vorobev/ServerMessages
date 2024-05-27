#include "server.h"


Server::Server()
    : QObject(nullptr)
    , m_server(nullptr)
{ }

Server::Server(QObject *parent)
    : QObject(parent)
    , m_server(nullptr)
{
}

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
    m_server = new QTcpServer(this);


    connect(m_server, &QTcpServer::newConnection, [&]() {
        auto client = m_server->nextPendingConnection();

        connect(client, &QTcpSocket::readyRead, [=]() {

            QStringList messageParts = QString::fromUtf8(client->readAll()).split("\n");
            QString operation = messageParts.value(0).trimmed();





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

        connect(client, &QTcpSocket::disconnected, [=]() {
            if (client->state() == QAbstractSocket::UnconnectedState) {
                client->deleteLater();
            }
        });
    });

    if (!m_server->listen(QHostAddress::Any, 4242))  // Используйте нужный вам порт
        qDebug("Не удалось запустить сервер");
    else
        qDebug("Сервер успешно запущен");
}


