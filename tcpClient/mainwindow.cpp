#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mytcpsocket = new KFileTransferSender();
    connect(mytcpsocket, SIGNAL(sendFileSize(qint64)), this, SLOT(onSendFileSize(qint64)));
    connect(mytcpsocket, SIGNAL(recvSize(qint64)), this, SLOT(onRecvSize(qint64)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn_connect_clicked()
{
    //连接服务器
    mytcpsocket->connect_to_server();
}

void MainWindow::on_btn_selectFile_clicked()
{
    //打开文件
    QString filepath = QFileDialog::getOpenFileName(this, "open", "../");
    ui->lineEdit->setText(filepath);
    if(!filepath.isEmpty())
        mytcpsocket->set_file(filepath);
}

void MainWindow::on_btn_upFile_clicked()
{
    //上传
    mytcpsocket->send_command(FILE_HEAD_CODE);
}

void MainWindow::on_btn_cancel_clicked()
{
    //取消上传
    mytcpsocket->send_command(FILE_CANCEL);
}

void MainWindow::onSendFileSize(qint64 size)
{
    size /= 1024;
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(size);
}

void MainWindow::onRecvSize(qint64 recvSize)
{
    recvSize /= 1024;
    ui->progressBar->setValue(recvSize);
}
