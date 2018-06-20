#include "chatServer.h"
#include <QApplication>
#include <QtSql/QSqlDatabase>
#include <QString>
#include <QSqlQuery>
#include <connectionpool.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
/*
    QSqlDatabase::addDatabase("QMYSQL");
    qDebug() << QSqlDatabase::drivers();
    qDebug() << QCoreApplication::libraryPaths();
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("chat_server");
    db.setUserName("root");
    db.setPassword("zenghui19980621.");
    db.setPort(3306);

    if (db.open()) {
        qDebug() << "Database is established!" << endl;
        QSqlQuery query(db);
        query.exec("SELECT * from user where username = 'hzeng'");
        while (query.next()) {
            qDebug() << query.value("password");
        }
    }
    else {
        qDebug() << "build Error" << endl;
        return a.exec();
    }
*/
/*
    QSqlDatabase db = ConnectionPool::openConnection();
    QSqlQuery query(db);
    QString var = "hzeng";
    query.prepare("select * from user where username = ?");
    query.addBindValue(var);
    query.exec();
    while (query.next()) {
        qDebug() << query.value("username").toString();
    }
    query.prepare("select * from user");
    query.exec();
    while (query.next()) {
        qDebug() << query.value(0) << query.value(1) << endl;
    }
    ConnectionPool::closeConnection(db);
*/
    myServer w;
    w.show();

    return a.exec();
}
