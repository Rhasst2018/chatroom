#include "global.h"
#include "dialog.h"
#include "login.h"
#include "changepassword.h"
#include "reg.h"
#include "chat.h"
#include <QSettings>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSettings *configIniWrite = new QSettings("config.ini", QSettings::IniFormat);
    configIniWrite->setValue("ip/first","10.189.234.70");
    configIniWrite->setValue("ip/second","0.0.0.0");
    configIniWrite->setValue("port/first","12345");
    delete configIniWrite;

    //Dialog w;

    login wLogin;
    changepassword wChange;
    reg wReg;
    Chat wChat;
    wLogin.show();

    QObject::connect(&wLogin, SIGNAL(showReg()), &wReg, SLOT(receiveShow()));
    QObject::connect(&wLogin, SIGNAL(showChange()), &wChange, SLOT(receiveShow()));
    QObject::connect(&wLogin, SIGNAL(quit()),&a, SLOT(quit()));
    QObject::connect(&wLogin, SIGNAL(callChat()), &wChat, SLOT(receiverShow()));
    QObject::connect(&wChange, SIGNAL(quit()), &a, SLOT(quit()));
    QObject::connect(&wReg, SIGNAL(quit()), &a, SLOT(quit()));
    QObject::connect(&wReg, SIGNAL(showLogin()), &wLogin, SLOT(receiveShow()));
    QObject::connect(&wChat, SIGNAL(quit()), &a, SLOT(quit()));

    return a.exec();
}
