#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "chatclient.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_sendbutton_clicked();

    void on_logoutbutton_clicked();

    void on_loginbutton_clicked();
    void connectedToServer();
    void messageReceived(const QString &sender,const QString &text);
    void jsonToReceived(const QJsonObject &docObj);
    void userJoined(const QString &user);
    void userLeft(const QString &user);
    void userListReceived(const QStringList &list);

private:
    Ui::MainWindow *ui;
    ChatClient *m_chatClient;
};
#endif // MAINWINDOW_H
