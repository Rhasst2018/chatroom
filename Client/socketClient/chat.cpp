#include "chat.h"
#include "ui_chat.h"
#include "global.h"
#include <QTimer>
#include <QDateTime>
#include <QFont>


Chat::Chat(QWidget *parent):QWidget(parent, Qt::Dialog | Qt::FramelessWindowHint), ui(new Ui::Chat)
{
    ui->setupUi(this);
    ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    fileName = "";
    QSettings *configIniRead = new QSettings("config.ini", QSettings::IniFormat);
    QString serverIp = configIniRead->value("ip/first").toString();
    QString port = configIniRead->value("port/first").toString();
    connect(ui->listWidget->model(), SIGNAL(rowsInserted(QModelIndex,int,int)), ui->listWidget, SLOT(scrollToBottom()));

    //连接
    delete configIniRead;
    socket = new QTcpSocket();
    socket->connectToHost(serverIp, quint16(port.toUInt()));

    connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));

    qDebug() << serverIp << quint16(port.toUInt()) << endl;
    if (!socket->waitForConnected(3000)) {
        QMessageBox::warning(this, "Error", socket->errorString());
        onConnected = false;
    }else{
        onConnected = true;

    }
}

Chat::~Chat()
{
    delete ui;
}
/*
bool Chat::eventFilter(QObject *target, QEvent *event)
{
    if(target == ui->messageEdit)
    {
        if(event->type() == QEvent::KeyPress)//回车键
        {
             QKeyEvent *k = static_cast<QKeyEvent *>(event);
             if(k->key() == Qt::Key_Return)
             {
                 on_sendButton_clicked();
                 return true;
             }
        }
    }
    return QWidget::eventFilter(target, event);
}
*/
void Chat::mousePressEvent(QMouseEvent *event)
{
    mMoveing = true;
    //记录下鼠标相对于窗口的位置
    //event->globalPos()鼠标按下时，鼠标相对于整个屏幕位置
    //pos() this->pos()鼠标按下时，窗口相对于整个屏幕位置
    mMovePosition = event->globalPos() - pos();
    return QWidget::mousePressEvent(event);
}

//重写鼠标移动事件
void Chat::mouseMoveEvent(QMouseEvent *event)
{
    //(event->buttons() && Qt::LeftButton)按下是左键
    //通过事件event->globalPos()知道鼠标坐标，鼠标坐标减去鼠标相对于窗口位置，就是窗口在整个屏幕的坐标
    if (mMoveing && (event->buttons() && Qt::LeftButton)
        && (event->globalPos()-mMovePosition).manhattanLength() > QApplication::startDragDistance())
    {
        move(event->globalPos()-mMovePosition);
        mMovePosition = event->globalPos() - pos();
    }
    return QWidget::mouseMoveEvent(event);
}

void Chat::mouseReleaseEvent(QMouseEvent *event)
{
    mMoveing = false;
}

void Chat::on_closeButton_2_clicked()
{
    emit quit();
}

void Chat::on_minButton_2_clicked()
{
    //托盘信息相关
    this->hide();
    QIcon icon = QIcon(":/chat-icon.png");
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(icon);
    trayIcon->setToolTip("Chatroom"); //提示文字
    connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason)));
    trayIcon->show();
}

void Chat::on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason){
    case QSystemTrayIcon::Trigger:
        //单击托盘图标
        break;
    case QSystemTrayIcon::DoubleClick:
        //双击托盘图标
        //双击后显示主程序窗口
        this->show();
        trayIcon->hide();
        break;
    default:
        break;
    }
}

void Chat::on_sendButton_clicked()
{
    if(ui->messageEdit->toPlainText().isEmpty() || onConnected == false)
        return;
    if(socket){
        QString mymessage = ui->messageEdit->toPlainText();
        QJsonObject request;
        request.insert("type", "message");
        request.insert("username", *getGlobalUsername());
        // modify the message
        if(mymessage.length() >= 30)
            mymessage.insert(30, "\n");

        request.insert("content", mymessage);
        //send to server
        socket->write(QJsonDocument(request).toJson());
        socket->flush();
        socket->waitForBytesWritten(3000);

        //clear text in chat
        ui->messageEdit->setPlainText("");
    }
}

void Chat::readyRead()
{
    QByteArray data = socket->readAll();
    qDebug() << data << endl;
    QJsonObject response= QJsonDocument::fromJson(data).object();
    if (response.value("type").toString() == "feedback") {
        if (response.value("state").toString() == "false") {
            QMessageBox::warning(this, "Wrong", response.value("tips").toString());
        }
        else {
            QMessageBox::information(this, "succeed", "操作成功");
        }
    }
    else if(response.value("type").toString() == "message"){
        QString timeNow = QTime::currentTime().toString("hh : mm");
        QFont f1("Microsoft YaHei", 7, 75);
        QFont f2("Microsoft YaHei", 10, 75);
        if(response.value("username").toString() == *getGlobalUsername()){
            QListWidgetItem *time_item = new QListWidgetItem(ui->listWidget);
            time_item->setTextAlignment(Qt::AlignRight);
            time_item->setTextColor(QColor(144, 255, 71));
            time_item->setFont(f1);
            time_item->setText(timeNow+" #"+response.value("username").toString());
            ui->listWidget->addItem(time_item);
            QListWidgetItem *new_item = new QListWidgetItem(ui->listWidget);
            new_item->setFont(f2);
            new_item->setTextAlignment(Qt::AlignRight);
            new_item->setTextColor(QColor(144, 255, 71));
            new_item->setText(toEmotion(response.value("content").toString()));
            ui->listWidget->addItem(new_item);
        }else{
            QListWidgetItem *time_item = new QListWidgetItem(ui->listWidget);
            time_item->setTextAlignment(Qt::AlignLeft);
            time_item->setTextColor(QColor(0, 0, 0));
            time_item->setFont(f1);
            time_item->setText(timeNow+" #"+response.value("username").toString());
            ui->listWidget->addItem(time_item);
            QListWidgetItem *new_item = new QListWidgetItem(ui->listWidget);
            new_item->setFont(f2);
            new_item->setTextAlignment(Qt::AlignLeft);
            new_item->setTextColor(QColor(0, 0, 0));
            new_item->setText(response.value("content").toString());
            ui->listWidget->addItem(new_item);
        }
    }
    else if(response.value("type").toString() == "files" ){
        if(response.value("username").toString() != *getGlobalUsername()){
            QString recvFileName = response.value("filename").toString();
            QByteArray recvContent(response.value("filecontent").toString().toLatin1());
            QString recvfilePath = QFileDialog::getSaveFileName(this, "Save", "../"+recvFileName, recvFileName.mid(recvFileName.indexOf(".")));
            qint64 fileSize = recvContent.size();

            QFile recvfile;
            recvfile.setFileName(recvfilePath);
            if(false == recvfile.open(QIODevice::WriteOnly)){
                QMessageBox::information(this, "Error", "Cannot open files");
            }else{
                recvfile.write(recvContent);
                // Only small files
            }
        }
    }
}

void Chat::receiverShow()
{
    this->show();
    QJsonObject request;
    request.insert("type", "message");
    request.insert("username", *getGlobalUsername());
    request.insert("content", *getGlobalUsername()+QString::fromLocal8Bit("已经进入聊天室"));
    socket->write(QJsonDocument(request).toJson());
    socket->flush();
    socket->waitForBytesWritten(3000);
}

void Chat::on_fileButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "open", "../");
    if(false == filePath.isEmpty()){
        fileName.clear();
        QFileInfo info(filePath);
        file.setFileName(filePath);
        if(false == file.open(QIODevice::ReadOnly)){
            qDebug() << QString::fromLocal8Bit("只读方式打开文件失败");
        }
        fileName = info.fileName();
        qDebug() << fileName;
        //fileSize = info.size();
        QByteArray buffer = file.readAll();     // 小文件
        if(socket){
            QJsonObject greeting;
            greeting.insert("type", "message");
            greeting.insert("username", *getGlobalUsername());
            greeting.insert("content", *getGlobalUsername()+QString::fromLocal8Bit("发送文件")+fileName);
            socket->write(QJsonDocument(greeting).toJson());
            socket->flush();
            socket->waitForBytesWritten(3000);
            QJsonObject request;
            request.insert("type", "files");
            request.insert("username", *getGlobalUsername());
            request.insert("filename", fileName);
            request.insert("filecontent", buffer.data());
            socket->write(QJsonDocument(request).toJson());
            socket->flush();
            socket->waitForBytesWritten(3000);
        }
    }
}

void Chat::bytesWritten(qint64 bytes)
{
    qDebug() << "Bytes Written: " << bytes;
}


void Chat::on_messageEdit_textChanged()
{
    if(ui->messageEdit->toPlainText().contains("\n")){
        on_sendButton_clicked();
    }
}

QString Chat::toEmotion(QString plain)
{
    if(plain.contains("\\happy"))
        plain.replace("\\happy", QString::fromLocal8Bit("(*^O^*)"));
    if(plain.contains("\\sad"))
        plain.replace("\\sad", QString::fromLocal8Bit("(ToT)/~~~"));
    if(plain.contains("\\zzz"))
        plain.replace("\\zzz", QString::fromLocal8Bit("(_ _).oO"));
    while(plain.endsWith("\n"))
        plain.replace("\n","");

    return plain;
}
