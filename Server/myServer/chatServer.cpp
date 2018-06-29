#include "chatServer.h"
#include "ui_myserver.h"

myServer::myServer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::myServer)
{
    ui->setupUi(this);
    this->setWindowTitle("Server");
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnect()));
}

myServer::~myServer()
{
    delete ui;
}

void myServer::on_ip_cursorPositionChanged(int arg1, int arg2)
{
    if (ui->ip->text().isEmpty() || ui->ip->text().length() < 7) {
        setButtonServer("close");
    } else {
        setButtonServer("open");
    }
}

//open server
void myServer::on_btnStart_clicked()
{
    if (server)
    {
        listening();
    } else {
        server = new QTcpServer(this);
        connect(server, SIGNAL(newConnection()), this, SLOT(newConnect()));
        listening();
    }
}

//new listening my server
void myServer::listening()
{
    if (!server->listen(QHostAddress::Any, ui->port->value())) {
        QMessageBox::warning(this, "Error", "Serser was open in address: " + ui->ip->text() + ":" + ui->port->text());
    } else {
        QMessageBox::information(this, "Successfully", "Server open successfully !");
        setButtonServer("open");
    }
}

//close server
void myServer::on_btnClose_clicked()
{
    //send to clients
    foreach(QTcpSocket *socket, connects) {
        socket->write("<< server was close >>");
        socket->flush();
        socket->waitForBytesWritten(3000);
    }

    //del server
    server->close();
    delete server;
    server = NULL;

    //del list connected
    connects.clear();

    QMessageBox::information(this, "Close", "Server was close!");
    setButtonServer("close");
}

void myServer::setButtonServer(QString is)
{
    //server was open
    if (is == "open") {
        ui->btnStart->setEnabled(false);
        ui->btnClose->setEnabled(true);
    } else if (is == "close") {
        ui->btnStart->setEnabled(true);
        ui->btnClose->setEnabled(false);
    }
}

//new client connecting...
void myServer::newConnect()
{
    QTcpSocket *socket = server->nextPendingConnection();

    connects.append(socket);

    //receive message from client
    connect(socket, SIGNAL(readyRead()), this, SLOT(receiveMessage()));
}

bool verify(QString username, QString password, QSqlQuery &query)
{
    query.prepare("select password from user where username = ?");
    query.addBindValue(username);
    query.exec();
    while (query.next()) {
        return (query.value("password").toString() == password);
    }
    return 0;
}

//get message from a client
void myServer::receiveMessage()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());

    QByteArray data = socket->readAll();
    QByteArray res;
    QJsonObject JsonData = QJsonDocument::fromJson(data).object();
    QJsonObject response;

    QSqlDatabase connection_query = ConnectionPool::openConnection();
    QSqlDatabase connection_insert = ConnectionPool::openConnection();
    QSqlQuery query(connection_query);
    QSqlQuery insert(connection_insert);

    switch (message_type.indexOf(JsonData.value("type").toString())) {

    case 0: // register
        response.insert("type", "feedback");
        query.prepare("select count(*) as num from user where username = ?");
        query.addBindValue(JsonData.value("username").toString());
        query.exec();
        while (query.next()) {
            if (query.value("num").toBool()) {
                response.insert("state", "false");
                response.insert("tips", "用户名已存在");
            }
            else {
                response.insert("state", "true");
            }
        }       
        if (response.value("state").toString() == "true") {
            insert.prepare("insert into user value(?, ?)");
            insert.addBindValue(JsonData.value("username").toString());
            insert.addBindValue(JsonData.value("password").toString());
            insert.exec();
            qDebug() << "insert into database" << endl;
        }
        //返回 res
        break;

    case 1: // login
        response.insert("type", "feedback");
        if (verify(JsonData.value("username").toString(), JsonData.value("password").toString(), query)) {
            response.insert("state", "true");
        }
        else {
            response.insert("state", "false");
            response.insert("tips", "用户名或密码错误");
        }
        break;

    case 2: //modify
        response.insert("type", "feedback");
        if (verify(JsonData.value("username").toString(), JsonData.value("oldpassword").toString(), query)) {
            response.insert("state", "true");
        }
        else {
            response.insert("state", "false");
            response.insert("tips", "原密码不正确或用户名错误");
        }
        if (response.value("state").toString() == "true") {
            insert.prepare("update user set password = ? where username = ?");
            insert.addBindValue(JsonData.value("newpassword").toString());
            insert.addBindValue(JsonData.value("username").toString());
            insert.exec();
        }
        break;

    case 6:
        response.insert("type", "message");
        response.insert("username", JsonData.value("username").toString());
        response.insert("content", JsonData.value("content").toString());
        res = QJsonDocument(response).toJson();
        foreach(QTcpSocket *socket, connects) {
            socket->write(res);
            socket->flush();
            socket->waitForBytesWritten(3000);
        }
        break;

    case 10:
        response.insert("type", "files");
        response.insert("filename", JsonData.value("filename").toString());
        response.insert("username", JsonData.value("username").toString());
        response.insert("filecontent", JsonData.value("filecontent").toString());
        res = QJsonDocument(response).toJson();
        foreach(QTcpSocket *socket, connects) {
            socket->write(res);
            socket->flush();
            socket->waitForBytesWritten(3000);
        }
        break;
    }

    if (response.value("type").toString() != "message" && response.value("type").toString() != "files") {
        res = QJsonDocument(response).toJson();
        socket->write(res);
        socket->flush();
        socket->waitForBytesWritten(3000);
    }

    qDebug() << res << endl << JsonData << endl;

    ConnectionPool::closeConnection(connection_insert);
    ConnectionPool::closeConnection(connection_query);
}

