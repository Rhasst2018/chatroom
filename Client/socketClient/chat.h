#ifndef CHAT_H
#define CHAT_H

#include <QWidget>
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
#include <QFileDialog>
#include <QFile>
#include <QListWidgetItem>

namespace Ui {
class Chat;
}

class Chat : public QWidget
{
    Q_OBJECT

public:
    explicit Chat(QWidget *parent = 0);
    ~Chat();
private slots:
    void on_closeButton_2_clicked();

    void on_minButton_2_clicked();

    void on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);

    void on_sendButton_clicked();

    void receiverShow();

    void on_fileButton_clicked();

    void bytesWritten(qint64);

    void on_messageEdit_textChanged();

    void on_listWidget_itemClicked(QListWidgetItem *item);

public slots:
    void readyRead();

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

signals:
    void quit();

private:
    Ui::Chat* ui;

    QString toEmotion(QString plain);

private:
    bool        mMoveing;
    QPoint      mMovePosition;
    QSystemTrayIcon *trayIcon;
    QTcpSocket *socket;
    bool        onConnected;
    QString     fileName;
    QFile       file;
};

#endif // CHAT_H
