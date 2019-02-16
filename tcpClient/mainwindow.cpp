#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mytcpsocket = new KFileTransferSender();
    connect(mytcpsocket, SIGNAL(progressValue(int)), this, SLOT(on_progressValueChanged(int)));

    ui->btn_upFile->setEnabled(false);
    ui->btn_cancel->setEnabled(false);
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
    {
        bool bOk = mytcpsocket->set_file(filepath);
        ui->btn_upFile->setEnabled(bOk);
    }
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

void MainWindow::on_progressValueChanged(int progressVal)
{
    ui->progressBar->setValue(progressVal);

    ui->btn_cancel->setEnabled(progressVal);
}
