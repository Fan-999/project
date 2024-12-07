#include "chatclient.h"
#include <QDataStream>
#include <QJsonObject>
#include <QJsonDocument>

ChatClient::ChatClient(QObject *parent)
    : QObject{parent}
{
    m_clientSocket=new QTcpSocket(this);
    connect(m_clientSocket,&QTcpSocket::connected,this,&ChatClient::connected);
    connect(m_clientSocket,&QTcpSocket::readyRead,this,&ChatClient::onReadyRead);
}

void ChatClient::onReadyRead()
{
    QByteArray jsonData;
    QDataStream socketStream(m_clientSocket);
    socketStream.setVersion(QDataStream::Qt_5_12);
    for(;;){
        socketStream.startTransaction(); //开始读取数据
        socketStream >> jsonData;
        //读取数据成功
        if(socketStream.commitTransaction()){
            // emit messageRecevied(QString::fromUtf8(jsonData));
            QJsonParseError paseError;
            const QJsonDocument jsonDoc=QJsonDocument::fromJson(jsonData,&paseError);
            if(paseError.error==QJsonParseError::NoError){
                if(jsonDoc.isObject()){
                    // emit logMessage(QJsonDocument(jsonDoc).toJson(QJsonDocument::Compact));
                    emit jsonReceived(jsonDoc.object());
                }
            }
        }else{
            break;
        }
    }
}

void ChatClient::sendMessage(const QString &text, const QString &type)
{
    if(m_clientSocket->state()!=QAbstractSocket::ConnectedState)
        return;
    if(!text.isEmpty()){
        QDataStream serverStream(m_clientSocket);
        serverStream.setVersion(QDataStream::Qt_5_12);
        //创建想要发送的json
        QJsonObject message;
        message["type"]=type;
        message["text"]=text;
        //发送json数据
        serverStream << QJsonDocument(message).toJson();
    }
}

void ChatClient::connectToServer(const QHostAddress &address, quint16 port)
{
    m_clientSocket->connectToHost(address,port);
}
//断开服务器
void ChatClient::disconnectFromHost()
{
    m_clientSocket->disconnectFromHost();
}
