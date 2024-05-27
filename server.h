#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>
#include <QTextCodec>
#include "usermanager.h"
#include "searchmanager.h"
#include "databasemanager.h"

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server();
    explicit Server(QObject *parent);
    ~Server();

    void startServer();

private:
    QTcpServer *m_server;
    UserManager m_userManager;
    DatabaseManager m_dbManager;
};

#endif // SERVER_H
