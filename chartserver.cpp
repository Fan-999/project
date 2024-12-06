#include "chartserver.h"
#include "serverworker.h"
#include <QJsonValue>
#include <QJsonObject>

ChartServer::ChartServer(QObject *parent)
    : QTcpServer{parent}
{}

void ChartServer::incomingConnection(qintptr socketDescriptor)
{
    ServerWorker *worker=new ServerWorker(this);
    if(!worker->setSocketDescriptor(socketDescriptor)){
        worker->deleteLater();
        return;
    }
    connect(worker,&ServerWorker::logMessage,this,&ChartServer::logMessage);
    connect(worker,&ServerWorker::jsonReceived,this,&ChartServer::jsonReceived);
    m_clients.append(worker);
    emit logMessage("新的用户连接上了");
}

void ChartServer::broadcast(const QJsonObject &message, ServerWorker *exclude)
{
    //对当前连接服务器的所有客户
    for(ServerWorker *worker:m_clients){
        worker->sendJson(message);
    }
}

void ChartServer::stopServer()
{
    close();
}

void ChartServer::jsonReceived(ServerWorker *sender, const QJsonObject &docObj)
{
    //获取聊天框type值
    const QJsonValue typeVal=docObj.value("type");
    if(typeVal.isNull()||!typeVal.isString())
        return; //空值或无效值，非法退出
    //如果type是消息
    if(typeVal.toString().compare("meaasge",Qt::CaseSensitive)==0){
        //获取消息text的正文
        const QJsonValue textVal=docObj.value("text");
        if(textVal.isNull()||!textVal.isString())
            return;
        const QString text=textVal.toString().trimmed();
        if(text.isEmpty())
            return;
        QJsonObject message;
        message["type"]="message";
        message["text"]=text;
        message["sender"]=sender->userName(); //发送消息的用户名
        broadcast(message,sender);
    }
    //如果type是login
    else if(typeVal.toString().compare("login",Qt::CaseInsensitive)==0){
        const QJsonValue usernameVal=docObj.value("text");
        if(usernameVal.isNull()||!usernameVal.isString())
            return; //空值或无效值，非法退出
        sender->setUserName(usernameVal.toString());
        QJsonObject connectedMessage;
        connectedMessage["type"]="newuser";
        connectedMessage["username"]=usernameVal.toString();
        broadcast(connectedMessage,sender);
    }
}
