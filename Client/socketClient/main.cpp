#include "global.h"
#include "dialog.h"
#include "login.h"
#include <QSettings>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSettings *configIniWrite = new QSettings("config.ini", QSettings::IniFormat);
    configIniWrite->setValue("ip/first","127.0.0.1");
    configIniWrite->setValue("ip/second","0.0.0.0");
    configIniWrite->setValue("port/first","12345");
    delete configIniWrite;

    Dialog w;
    w.show();

    login wLogin;
    QObject::connect(&w,SIGNAL(showLogin()),&wLogin,SLOT(receiveShow()));
    QObject::connect(&wLogin,SIGNAL(showMain()),&w,SLOT(receiveShow()));
    QObject::connect(&w,SIGNAL(quit()),&a,SLOT(quit()));
    QObject::connect(&wLogin,SIGNAL(quit()),&a,SLOT(quit()));

    return a.exec();
}
