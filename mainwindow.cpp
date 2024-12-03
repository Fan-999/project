#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startStopbutton_clicked()
{

    static bool started=false;
    if(started){

        ui->startStopbutton->setText("停止服务器");
        logMessage("服务器已经启动");
    }
    else{
        ui->startStopbutton->setText("启动服务器");
        logMessage("服务器已经停止");
    }
    started=!started;

}

void MainWindow::logMessage(const QString &msg)
{
    ui->logEdit->appendPlainText(msg);
}

