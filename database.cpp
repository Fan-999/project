#include "database.h"
#include <QtSql>
#include <QSqlDatabase>


database::database(QObject *parent)
    : QObject{parent}
{
    ininDatabase();
}


//连接数据库
void database::ininDatabase()
{
    //打开数据库
    idatabase=QSqlDatabase::addDatabase("QSQLITE");//添加数据库驱动;
    QString aFile="D:/2024年秋/QT/实验报告/lab4.db";
    idatabase.setDatabaseName(aFile);
    if(!idatabase.open())    //打开数据库
    {
        qDebug()<<"数据库打开失败";
    }
    else
        qDebug()<<"成功打开数据库";
}
//实现登录控制
QString database::userLogin(QString userName, QString password, QString role)
{
    QSqlQuery query;
    //根据传过来的role进入不同的表进行用户验证
    if(role == "user") {
        query.prepare("select username,password from users where username=:USERS");
    } else if(role == "admin") {
        query.prepare("select username,password from admin where username=:USERS");
    }
    query.bindValue(":USERS", userName);
    query.exec();
    if(query.first() && query.value("username").isValid()) {
        QString passwd = query.value("password").toString();
        if(passwd == password) {
            qDebug() << "Welcome";
            return "loginOK";
        } else {
            qDebug() << "error password";
            return "wrongPassword";
        }
    } else {
        qDebug() << "no such user";
        return "wrongUsername";
    }
    // query.prepare("select username,password from users where username=:USERS");
    // query.bindValue(":USERS",userName);
    // query.exec();
    // if(query.first()&&query.value("username").isValid()){
    //     QString passwd=query.value("password").toString();
    //     if(passwd==password){
    //         qDebug()<<"Welcome";
    //         return "loginOK";
    //     }
    //     else{
    //         qDebug()<<"error password";
    //         return "wrongPassword";
    //     }
    // }
    // else{
    //     qDebug()<<"no such user";
    //     return "wrongUsername";
    // }
}
