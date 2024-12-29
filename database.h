#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QtSql>
#include <QSqlDatabase>

class database : public QObject
{
    Q_OBJECT
public:
    static database &getInstance()
    {
        static database instance;
        return instance;
    }
    //登录实现逻辑
    QString userLogin(QString userName,QString password,QString role);

private:
    explicit database(QObject *parent = nullptr);
    database(database const &)=delete;
    void operator=(database const &)=delete;
    QSqlDatabase idatabase;
    void ininDatabase();//连接数据库
signals:
};

#endif // DATABASE_H
