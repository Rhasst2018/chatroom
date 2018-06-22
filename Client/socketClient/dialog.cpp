#include "dialog.h"
#include "global.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Client");
    socket = new QTcpSocket();

    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
    connect(ui->chatLog->model(), SIGNAL(rowsInserted(QModelIndex,int,int)),
            ui->chatLog, SLOT(scrollToBottom()));
}

void Dialog::receiveShow()
{
    this->show();
}

//is connected to server
void Dialog::connected()
{
    ui->chatLog->clear();
    ui->btnSend->setEnabled(true);
}

//disconnect to server
void Dialog::disconnected()
{
    ui->btnSend->setEnabled(false);
    QMessageBox::warning(this, "Dissconnect", "Oops! Loss connect to server");

   //reconnecting
    ui->btnConnecter->setEnabled(true);
}

void Dialog::readyRead()
{
    QByteArray data= socket->readAll();
    QJsonObject response= QJsonDocument::fromJson(data).object();
    if (response.value("type").toString() == "feedback") {
        if (response.value("state").toString() == "false") {
            QMessageBox::warning(this, "Wroing", response.value("tips").toString());
        }
        else {
            QMessageBox::information(this, "succeed", "操作成功");
        }
    }
    else
        ui->chatLog->addItem(response.value("content").toString());
}

void Dialog::bytesWritten(qint64 bytes)
{
    qDebug() << "Bytes Written: " << bytes;
}

Dialog::~Dialog()
{
    if (socket) {
        QString message = "<" +ui->nickname->text() + "> was left";
        QJsonObject request;
        request.insert("type", "message");
        request.insert("content", message);
        //send to server
        socket->write(QJsonDocument(request).toJson());
        socket->flush();
        socket->waitForBytesWritten(3000);
    }
    delete ui;
}

//send message
void Dialog::on_btnSend_clicked()
{
    if (ui->message->text().isEmpty()) {
        QMessageBox::warning(this, "Wrong", "message cannot be empty");
        return;
    }
    if (socket) {
        QString message = "#" +ui->nickname->text() + ": " + ui->message->text();
        QJsonObject request;
        request.insert("type", "message");
        request.insert("content", message);
        //send to server
        socket->write(QJsonDocument(request).toJson());
        socket->flush();
        socket->waitForBytesWritten(3000);

        //clear text in chat
        ui->message->setText("");
    }
}

//connecting to server
void Dialog::on_btnConnecter_clicked()
{
    socket->connectToHost(ui->server->text(), ui->port->value());

    if (!socket->waitForConnected(3000)) {
        QMessageBox::warning(this, "Error", socket->errorString());
    } else {
        QMessageBox::information(this, "Successfuly", QString("Connected to Server  " + ui->server->text() + ":" + ui->port->text()));
        ui->btnConnecter->setEnabled(false);
    }
}

//change text in input send message
void Dialog::on_message_cursorPositionChanged(int arg1, int arg2)
{
}

void Dialog::on_signup_clicked()
{
    if (ui->username->text().isEmpty() || ui->password->text().isEmpty()) {
        QMessageBox::warning(this, "Wrong", "Username or Password cannot empty");
        return;
    }

    if (socket) {
        QJsonObject request;
        request.insert("type", "register");
        request.insert("username", ui->username->text());
        request.insert("password", ui->password->text());
        //send to server
        socket->write(QJsonDocument(request).toJson());
        socket->flush();
        socket->waitForBytesWritten(3000);
    }

}

void Dialog::on_login_clicked()
{
    this->hide();
    emit showLogin();
    /*
    if (ui->nickname->text().isEmpty() || ui->login_password->text().isEmpty()) {
        QMessageBox::warning(this, "Wrong", "Username or Password cannot empty");
        return;
    }
    if (socket) {
        QJsonObject request;
        request.insert("type", "login");
        request.insert("username", ui->nickname->text());
        request.insert("password", ui->login_password->text());
        //send to server
        socket->write(QJsonDocument(request).toJson());
        socket->flush();
        socket->waitForBytesWritten(3000);
    }
    */
}
