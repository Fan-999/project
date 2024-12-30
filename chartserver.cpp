#include "chartserver.h"
#include "serverworker.h"
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>

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
    connect(worker,&ServerWorker::disconnectedFromClient,this,
            std::bind(&ChartServer::userDisconnected,this,worker));
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
    if(typeVal.toString().compare("message",Qt::CaseSensitive)==0){
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

        //向新连接服务器的用户发消息
        QJsonObject userListMessage;
        userListMessage["type"]="userlist";
        QJsonArray userlist;
        //获取连接到该服务器的所有用户名
        for(ServerWorker *worker:m_clients){
            if(worker==sender)
                userlist.append("👶🏻"+worker->userName());    //如果发送者是自己，加*号
            else
                userlist.append(worker->userName());
        }
        userListMessage["userlist"]=userlist;
        sender->sendJson(userListMessage);
    }
    // 如果type是移除命令
    else if(typeVal.toString().compare("remove", Qt::CaseInsensitive) == 0) {
        QString removedUsername = docObj.value("text").toString();
        qDebug()<<removedUsername ;
        // 广播移除消息给所有客户端，除了被移除的用户自己 并发送“removed”信号给客户端
        QJsonObject removedMessage;
        removedMessage["type"] = "removed";
        removedMessage["username"] = removedUsername;
        broadcast(removedMessage, sender);
        emit logMessage(removedUsername + "已被移除");
    }
    // 如果type是私聊
    else if (typeVal.toString().compare("private", Qt::CaseInsensitive) == 0) {
        QString sender = docObj.value("sender").toString();
        qDebug()<<sender;
        QString receiver = docObj.value("receiver").toString();
        QString text = docObj.value("text").toString();
        // 查找接收者并发送私聊消息
        for (ServerWorker *worker : m_clients) {
            if (worker->userName() == receiver||worker->userName() == sender) {
                qDebug()<<worker->userName() ;
                QJsonObject privateMessage;
                privateMessage["type"] = "privated";
                privateMessage["sender"] = sender;
                privateMessage["receiver"] = receiver;
                privateMessage["text"] = text;
                worker->sendJson(privateMessage);
                // break;
            }
        }

    }
}
//处理用户退出操作
void ChartServer::userDisconnected(ServerWorker *sender)
{
    m_clients.removeAll(sender);    //删除用户
    const QString userName=sender->userName();
    if(!userName.isEmpty()){
        QJsonObject disconnectedMessage;
        disconnectedMessage["type"]="userdisconnected"; //跟客户端发送删除消息
        disconnectedMessage["username"]=userName;
        broadcast(disconnectedMessage,nullptr);
        emit logMessage(userName + "已断开连接");
    }
    sender->deleteLater();
}
