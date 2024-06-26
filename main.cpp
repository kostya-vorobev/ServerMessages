#include <QCoreApplication>
#include <QTextCodec>
#include <QDebug>
#include <QtSql/QSqlError>
#include <QDir>
#include <QStandardPaths>
#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>
#include "server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextCodec *codec = QTextCodec::codecForName("CP 866");
    QTextCodec::setCodecForLocale(codec);
    Server myServer;
    myServer.startServer();

    return a.exec();
}
