#include "serverworker.h"
#include <QDataStream>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonDocument>

ServerWorker::ServerWorker(QObject *parent)
    : QObject{parent}
{
    m_serverSocket=new QTcpSocket(this);
    connect(m_serverSocket,&QTcpSocket::readyRead,this,&ServerWorker::onReadyRead);
}

bool ServerWorker::setSocketDescriptor(qintptr socketDescriptor)
{
    return m_serverSocket->setSocketDescriptor(socketDescriptor);
}
//获取username
QString ServerWorker::userName()
{
    return m_userName;
}
//设置username
void ServerWorker::setUserName(QString user)
{
    m_userName=user;
}

void ServerWorker::onReadyRead()
{
    QByteArray jsonData;
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_12);
    for(;;){
        socketStream.startTransaction(); //开始读取数据
        socketStream >> jsonData;
        //读取数据成功
        if(socketStream.commitTransaction()){
            emit logMessage(QString::fromUtf8(jsonData));
            sendMessage("I recieved message");  //发送回去

            QJsonParseError paseError;
            const QJsonDocument jsonDoc=QJsonDocument::fromJson(jsonData,&paseError);
            if(paseError.error==QJsonParseError::NoError){
                if(jsonDoc.isObject()){
                    emit logMessage(QJsonDocument(jsonDoc).toJson(QJsonDocument::Compact));
                    emit jsonReceived(this,jsonDoc.object());
                }
            }
        }else{
            break;
        }
    }
}

void ServerWorker::sendMessage(const QString &text, const QString &type)
{
    if(m_serverSocket->state()!=QAbstractSocket::ConnectedState)
        return;
    if(!text.isEmpty()){
        QDataStream serverStream(m_serverSocket);
        serverStream.setVersion(QDataStream::Qt_5_12);
        //创建想要发送的json
        QJsonObject message;
        message["type"]=type;
        message["text"]=text;
        //发送json数据
        serverStream << QJsonDocument(message).toJson();
    }
}

void ServerWorker::sendJson(const QJsonObject &json)
{
    const QByteArray jsonData=QJsonDocument(json).toJson(QJsonDocument::Compact);
    emit logMessseage(QLatin1String("Sending to")+userName()+QLatin1String("-")+QString::fromUtf8(jsonData));
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);
    socketStream << jsonData;
}
