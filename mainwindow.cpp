#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_chartServer=new ChartServer(this);

    connect(m_chartServer,&ChartServer::logMessage,this,&MainWindow::logMessage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startStopbutton_clicked()
{

    if(m_chartServer->isListening()){
        m_chartServer->stopServer();
        ui->startStopbutton->setText("启动服务器");
        logMessage("服务器已停止");
    }else{
        if(!m_chartServer->listen(QHostAddress::Any,1967)){
            QMessageBox::critical(this,"错误","无法启动服务器");
            return;
        }
        logMessage("服务器已经启动");
        ui->startStopbutton->setText("停止服务器");
    }
}

void MainWindow::logMessage(const QString &msg)
{
    ui->logEdit->appendPlainText(msg);
}

