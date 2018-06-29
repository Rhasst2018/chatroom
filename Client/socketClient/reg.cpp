#include "reg.h"
#include "ui_reg.h"

reg::reg(QWidget *parent) :
    QDialog(parent, Qt::Dialog| Qt::FramelessWindowHint),
    ui(new Ui::reg)
{
    ui->setupUi(this);
    ui->comboBox->setInsertPolicy(QComboBox::InsertAfterCurrent);
    ui->comboBox->setEditable(true);
    ui->comboBox->setAutoCompletion(true);
    ui->comboBox->setCurrentText("");
    ui->passwordEdit->setEchoMode(QLineEdit::Password);
    ui->passwordEdit->setTextMargins(1,1,QSize(26,26).width(),1);
    //初始化服务器连接
    socket = new QTcpSocket();
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));

    //鼠标拖动
    mMoveing = false;
}

reg::~reg()
{
delete ui;
}


void reg::on_regButton_clicked()
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
    QString name =ui->comboBox->currentText();
    QString password=ui->passwordEdit->text();

    QString pattern("[0-9a-zA-Z]+");
    QRegExp rx(pattern);
    if(password.contains(' ')){
        QMessageBox::warning(this, "Wrong", " Password cannot have space");
        return;
    }
    else if(password.length()>16||password.length()<8){
        QMessageBox::warning(this, "Wrong", " Password length should be 8-16");
        return;
    }
    else if(rx.exactMatch(name)==0){
        QMessageBox::warning(this, "Wrong", " name should only be a-z A-Z 0-9");
        return;

    }else{

    }
    if (!socket->waitForConnected(3000)) {
        QMessageBox::warning(this, "Error", socket->errorString());
    }
    else {
        QJsonObject request;
        request.insert("type", "register");
        request.insert("username",name);
        request.insert("password", password);
        //send to server
        socket->write(QJsonDocument(request).toJson());
        socket->flush();
        socket->waitForBytesWritten(3000);
    }
}
//重写鼠标按下事件
void reg::mousePressEvent(QMouseEvent *event)
{
    mMoveing = true;
    //记录下鼠标相对于窗口的位置
    //event->globalPos()鼠标按下时，鼠标相对于整个屏幕位置
    //pos() this->pos()鼠标按下时，窗口相对于整个屏幕位置
    mMovePosition = event->globalPos() - pos();
    return QDialog::mousePressEvent(event);
}

//重写鼠标移动事件
void reg::mouseMoveEvent(QMouseEvent *event)
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

void reg::mouseReleaseEvent(QMouseEvent *event)
{
    mMoveing = false;
}
void reg::on_closeButton_clicked()
{
    //this->hide();
    emit quit();
}
void reg::on_minButton_clicked()
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
void reg::on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason)
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

void reg::bytesWritten(qint64 bytes)
{
    qDebug() << "Bytes Written: " << bytes;
}
void reg::readyRead()
{
    QByteArray data= socket->readAll();
    QJsonObject response= QJsonDocument::fromJson(data).object();
    if (response.value("type").toString() == "feedback") {
        if (response.value("state").toString() == "false") {
            QMessageBox::warning(this, "Wroing", response.value("tips").toString());
        }
        else if(response.value("state").toString() == "true"){
            QMessageBox::information(this, "success", "Let's try");
            this->showLogin();
            this->hide();
        }else{
            printf("Damm\n");
            printf("%s\n",response.value("state").toString().toLatin1());
        }
    }
}

void reg::receiveShow()
{
    this->show();
}
