#include "login.h"
#include "global.h"
#include "ui_login.h"

login::login(QWidget *parent) :
    QDialog(parent, Qt::Dialog| Qt::FramelessWindowHint),
    ui(new Ui::login)
{
    ui->setupUi(this);
    ui->passwordEdit->setEchoMode(QLineEdit::Password);
    ui->passwordEdit->setTextMargins(1,1,QSize(26,26).width(),1);

    ui->comboBox->setInsertPolicy(QComboBox::InsertAfterCurrent);
    ui->comboBox->setEditable(true);
    //初始化登录列表
    QSettings *configIniRead = new QSettings("user.ini", QSettings::IniFormat);

    QStringList userList = configIniRead->childGroups();
    qDebug() << userList;
    foreach (QString user, userList) {
        QString username = configIniRead->value(user + "/username").toString();
        QString password = configIniRead->value(user + "/password", "").toString();
        ui->comboBox->addItem(username);
        userLogList.insert(username, password);
    }

    delete configIniRead;

    ui->comboBox->setAutoCompletion(true);
    ui->comboBox->setCurrentText("");
    //初始化服务器连接
    socket = new QTcpSocket();
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));

    //鼠标拖动
    mMoveing = false;
}

login::~login()
{
    delete ui;
}

void login::on_rememberPassword_stateChanged(int arg1)
{

}

void login::on_autoLogin_stateChanged(int arg1)
{

}

void login::on_loginButton_clicked()
{
    if (ui->comboBox->currentText().isEmpty()|| ui->passwordEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Wrong", "Username or Password cannot empty");
        return;
    }
    //获得配置
    QSettings *configIniRead = new QSettings("config.ini", QSettings::IniFormat);
    QString serverIp = configIniRead->value("ip/first").toString();
    QString port = configIniRead->value("port/first").toString();
    //连接
    socket->connectToHost(serverIp, quint16(port.toUInt()));
    qDebug() << serverIp << quint16(port.toUInt()) << endl;
    delete configIniRead;

    if (!socket->waitForConnected(3000)) {
        QMessageBox::warning(this, "Error", socket->errorString());
    }
    else {
        QJsonObject request;
        request.insert("type", "login");
        request.insert("username", ui->comboBox->currentText());
        request.insert("password", ui->passwordEdit->text());
        //send to server
        socket->write(QJsonDocument(request).toJson());
        socket->flush();
        socket->waitForBytesWritten(3000);
        //加跳转
    }
}

void login::receiveShow()
{
    this->show();
}

void login::on_registerLabel_clicked()
{
    this->hide();
    emit showMain();
}

//重写鼠标按下事件
void login::mousePressEvent(QMouseEvent *event)
{
    mMoveing = true;
    //记录下鼠标相对于窗口的位置
    //event->globalPos()鼠标按下时，鼠标相对于整个屏幕位置
    //pos() this->pos()鼠标按下时，窗口相对于整个屏幕位置
    mMovePosition = event->globalPos() - pos();
    return QDialog::mousePressEvent(event);
}

//重写鼠标移动事件
void login::mouseMoveEvent(QMouseEvent *event)
{
    //(event->buttons() && Qt::LeftButton)按下是左键
    //通过事件event->globalPos()知道鼠标坐标，鼠标坐标减去鼠标相对于窗口位置，就是窗口在整个屏幕的坐标
    if (mMoveing && (event->buttons() && Qt::LeftButton)
        && (event->globalPos()-mMovePosition).manhattanLength() > QApplication::startDragDistance())
    {
        move(event->globalPos()-mMovePosition);
        mMovePosition = event->globalPos() - pos();
    }
    return QDialog::mouseMoveEvent(event);
}

void login::mouseReleaseEvent(QMouseEvent *event)
{
    mMoveing = false;
}

void login::on_closeButton_clicked()
{
    emit quit();
}

void login::on_minButton_clicked()
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

void login::on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason)
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

void login::readyRead()
{
    QByteArray data= socket->readAll();
    QJsonObject response= QJsonDocument::fromJson(data).object();
    if (response.value("type").toString() == "feedback") {
        if (response.value("state").toString() == "false") {
            QMessageBox::warning(this, "Wroing", response.value("tips").toString());
        }
        else {
            qDebug() << userLogList << endl;
            *getGlobalUsername() = ui->comboBox->currentText();
            if (userLogList.value(ui->comboBox->currentText(), NULL) == NULL ||
                    (userLogList.value(ui->comboBox->currentText(), NULL) == "" &&
                     ui->rememberPassword->isChecked())) {
                QSettings *configIniWrite = new QSettings("user.ini", QSettings::IniFormat);
                QStringList userList = configIniWrite->childGroups();
                QString count = QString::number(userList.count(), 10);

                configIniWrite->setValue(count + "/username", ui->comboBox->currentText());
                if (ui->rememberPassword->isChecked()) {
                    configIniWrite->setValue(count + "/password", ui->passwordEdit->text());
                }
                delete configIniWrite;
            }
        }
    }
}

void login::bytesWritten(qint64 bytes)
{
    qDebug() << "Bytes Written: " << bytes;
}

void login::on_comboBox_currentIndexChanged(const QString &arg1)
{
    ui->passwordEdit->setText(userLogList[arg1]);
}
