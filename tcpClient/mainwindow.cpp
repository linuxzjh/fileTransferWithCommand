#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mytcpsocket = new KFileTransferSender(parent);
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
    mytcpsocket->connect_to_server(IP, PORT_COMMAND);
}

void MainWindow::on_btn_selectFile_clicked()
{
    //打开文件
    QString filepath = QFileDialog::getOpenFileName(this, "open", "../");
    ui->lineEdit->setText(filepath);
    if(!filepath.isEmpty())
    {
        ui->btn_upFile->setEnabled(true);
    }
}

void MainWindow::on_btn_upFile_clicked()
{
    //上传
    QString filePath = ui->lineEdit->text();
    if (! filePath.isEmpty())
    {
        mytcpsocket->sendFile(filePath);
    }
}

void MainWindow::on_btn_cancel_clicked()
{
    //取消上传
    mytcpsocket->unSendFile([this](int& argRet, QString& argAdditional) mutable {
        if(argRet)
        {
            QMessageBox::information(this, tr("取消上传"), argAdditional);
            //发送信号,通知外部做相应处理;
        }
    });
}

void MainWindow::on_progressValueChanged(int progressVal)
{
    ui->progressBar->setValue(progressVal);

    ui->btn_cancel->setEnabled(progressVal);
}

void MainWindow::on_btn_freeDiskCheck_clicked()
{
    QString filePath = ui->lineEdit->text();
    QFileInfo fileInfo(filePath);
    if (fileInfo.isFile())
    {
        mytcpsocket->isDiskFreeSpace(fileInfo.size(), [this](int& argRet, QString& argAdditional) {
            QMessageBox::information(this, tr("磁盘空间检查 "), argAdditional);
        });
    }
}

void MainWindow::on_btn_isExistFile_clicked()
{

}
