#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHostAddress>
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->widget->setCurrentWidget(ui->loginPage);
    m_chatClient=new ChatClient(this);
    connect(m_chatClient,&ChatClient::connected,this,&MainWindow::connectedToServer);
    // connect(m_chatClient,&ChatClient::messageRecevied,this,&MainWindow::messageReceived);
    connect(m_chatClient,&ChatClient::jsonReceived,this,&MainWindow::jsonToReceived);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//发送操作
void MainWindow::on_sendbutton_clicked()
{
    //将文本框消息发送
    if(!ui->saylineEdit->text().isEmpty())
        m_chatClient->sendMessage(ui->saylineEdit->text());
}

//退出操作
void MainWindow::on_logoutbutton_clicked()
{
    m_chatClient->disconnectFromHost(); //断开连接
    ui->widget->setCurrentWidget(ui->loginPage);

    for(auto aItem:ui->userlistWidget->findItems(ui->nameEdit->text(),Qt::MatchExactly)){
        qDebug("remove");
        ui->userlistWidget->removeItemWidget(aItem);    //移除
        delete aItem;
    }
}


//登录操作
void MainWindow::on_loginbutton_clicked()
{
    m_chatClient->connectToServer(QHostAddress(ui->serverEdit->text()),1967);
}


void MainWindow::connectedToServer()
{
    ui->widget->setCurrentWidget(ui->chatPage);
    //用户登录，发送消息
    m_chatClient->sendMessage(ui->nameEdit->text(),"login");
}

void MainWindow::messageReceived(const QString &sender, const QString &text)
{
    ui->roomtextEdit->append(QString("%1 : %2").arg(sender).arg(text));
}

void MainWindow::jsonToReceived(const QJsonObject &docObj)
{
    //获取聊天框type值
    const QJsonValue typeVal=docObj.value("type");
    if(typeVal.isNull()||!typeVal.isString())
        return; //空值或无效值，非法退出
    //如果type是消息
    if(typeVal.toString().compare("message",Qt::CaseSensitive)==0){
        //获取消息text的正文
        const QJsonValue textVal=docObj.value("text");
        const QJsonValue senderVal=docObj.value("sender");
        if(textVal.isNull()||!textVal.isString())
            return;
        if(senderVal.isNull()||!senderVal.isString())
            return;
        messageReceived(senderVal.toString(),textVal.toString());  //接收发送内容

    }
    //如果type是login
    else if(typeVal.toString().compare("newuser",Qt::CaseInsensitive)==0){
        const QJsonValue usernameVal=docObj.value("username");
        if(usernameVal.isNull()||!usernameVal.isString())
            return; //空值或无效值，非法退出
        userJoined(usernameVal.toString());
    }
    //如果type是userdisconnected
    else if(typeVal.toString().compare("userdisconnected",Qt::CaseInsensitive)==0){
        const QJsonValue usernameVal=docObj.value("username");
        if(usernameVal.isNull()||!usernameVal.isString())
            return; //空值或无效值，非法退出
        userLeft(usernameVal.toString());
    }
    //如果type是userlist
    else if(typeVal.toString().compare("userlist",Qt::CaseInsensitive)==0){
        const QJsonValue userlistVal=docObj.value("userlist");
        if(userlistVal.isNull()||!userlistVal.isArray())
            return; //空值或无效值，非法退出
        qDebug() << userlistVal.toVariant().toStringList();
        userListReceived(userlistVal.toVariant().toStringList());
    }
}

void MainWindow::userJoined(const QString &user)
{
    ui->userlistWidget->addItem(user);  //添加用户
}

void MainWindow::userLeft(const QString &user)
{
    //移除用户
    for(auto aItem:ui->userlistWidget->findItems(user,Qt::MatchExactly)){
        qDebug("remove");
        ui->userlistWidget->removeItemWidget(aItem);    //移除
        delete aItem;
    }
}
//更新新进服务器的客户的客户列表
void MainWindow::userListReceived(const QStringList &list)
{
    ui->userlistWidget->clear();
    ui->userlistWidget->addItems(list);
}

