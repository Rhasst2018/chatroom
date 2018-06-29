#ifndef CHANGEPASSWORD_H
#define CHANGEPASSWORD_H

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
class changepassword;
}

class changepassword : public QDialog
{


Q_OBJECT
private:
Ui::changepassword *ui;
QTcpSocket *socket;
bool        mMoveing;
QPoint      mMovePosition;
QSystemTrayIcon *trayIcon;
QMap<QString, QString> userLogList;
public:
explicit changepassword(QWidget *parent = 0);
~changepassword();
signals:
void showLogin();

void quit();
protected:
virtual void mousePressEvent(QMouseEvent *event);
virtual void mouseMoveEvent(QMouseEvent *event);
virtual void mouseReleaseEvent(QMouseEvent *event);

private slots:
void on_changepasswordButton_clicked();
void readyRead();
void on_minButton_clicked();
void bytesWritten(qint64);
void on_closeButton_clicked();
void on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);
void receiveShow();
};

#endif // CHANGEPASSWORD_H
