#ifndef CHARTSERVER_H
#define CHARTSERVER_H

#include <QObject>
#include <QTcpServer>
#include "serverworker.h"

class ChartServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit ChartServer(QObject *parent = nullptr);
protected:
    void incomingConnection(qintptr socketDescriptor) override;
    QVector<ServerWorker *>m_clients;   //记录所有用户连接

    void broadcast(const QJsonObject &message,ServerWorker *exclude);
signals:
    void logMessage(const QString& msg);
public slots:
    void stopServer();
    void jsonReceived(ServerWorker *sender,const QJsonObject &docObj);
    void userDisconnected(ServerWorker *sender);
};

#endif // CHARTSERVER_H
