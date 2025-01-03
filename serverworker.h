#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include <QObject>
#include <QTcpSocket>

class ServerWorker : public QObject
{
    Q_OBJECT
public:
    explicit ServerWorker(QObject *parent = nullptr);
    virtual bool setSocketDescriptor(qintptr socketDescriptor);

    QString userName(); //获取username
    void setUserName(QString user); //设置username
signals:
    void logMessage(const QString &msg);
    void jsonReceived(ServerWorker *sender,const QJsonObject &docObj);
    void disconnectedFromClient();
private:
    QTcpSocket  *m_serverSocket;
    QString m_userName;
public:
    void onReadyRead();
    void sendMessage(const QString &text,const QString &type="message");
    void sendMessages(const QString &sender,const QString &receiver,const QString &text,const QString &type="message");
    void sendJson(const QJsonObject &json);
};

#endif // SERVERWORKER_H
