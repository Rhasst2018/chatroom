#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QtGui/QMouseEvent>
#include <QSystemTrayIcon>
#include <QIcon>
#include <QAbstractSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QTcpSocket>
#include <QSize>
#include <QSettings>
#include <QMap>

namespace Ui {
class login;
}

class login : public QDialog
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = 0);
    ~login();

private slots:
    void on_rememberPassword_stateChanged(int arg1);

    void on_autoLogin_stateChanged(int arg1);

    void on_loginButton_clicked();

    void receiveShow();

    void on_registerLabel_clicked();

    void on_closeButton_clicked();

    void on_minButton_clicked();

    void on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);

private slots:
    void readyRead();

    void bytesWritten(qint64);

    void on_comboBox_currentIndexChanged(const QString &arg1);

signals:
    void showMain();

    void quit();

protected:

    virtual void mousePressEvent(QMouseEvent *event);

    virtual void mouseMoveEvent(QMouseEvent *event);

    virtual void mouseReleaseEvent(QMouseEvent *event);

private:
    Ui::login *ui;
    QTcpSocket *socket;
    bool        mMoveing;
    QPoint      mMovePosition;
    QSystemTrayIcon *trayIcon;
    QMap<QString, QString> userLogList;
};

#endif // LOGIN_H
