#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "kfiletransfercachemanage.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mytcpsocket = new KFileTransferSender(parent);
    connect(mytcpsocket, SIGNAL(progressValue(const QString&, int)), this, SLOT(on_progressValueChanged(const QString&,int)));
    connect(mytcpsocket, SIGNAL(errorState(int,int)), this, SLOT(on_error_state(int, int)));
    connect(mytcpsocket, &KFileTransferSender::fileTransferFinish, this, [this]{
       ui->progressBar->setValue(100);
       QMessageBox::information(this, "传输完成", "传输完成");
    });

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
    bool bconnected = mytcpsocket->connect_to_server(IP, PORT_COMMAND);
    qDebug() << (bconnected ? "连接成功" : "连接超时");
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
    mytcpsocket->cancelSendFile();
}

void MainWindow::on_progressValueChanged(const QString& fileName, int progressVal)
{
    qDebug() << "current transfer fileName===>" << fileName << ",progressVal===>" << progressVal;
    ui->progressBar->setValue(progressVal);

    ui->btn_cancel->setEnabled(progressVal);
}

void MainWindow::on_error_state(int code, int subCode)
{
    qDebug() << __FUNCTION__ << " error: code=" << code << "\tsubCode=" << subCode;
}

void MainWindow::on_btn_freeDiskCheck_clicked()
{
    QString filePath = ui->lineEdit->text();
    QFileInfo fileInfo(filePath);
    if (fileInfo.isFile())
    {
        bool bDiskFreeSpace = mytcpsocket->isDiskFreeSpace(fileInfo.size());
        qDebug() << (bDiskFreeSpace ? "缓存目录有多余的磁盘空间 " : "缓存目录没有多余的磁盘空间 ");
    }
}

void MainWindow::on_btn_isExistFile_clicked()
{
    QString filePath = ui->lineEdit->text();
    QFile file(filePath);
    if (file.exists())
    {
        checkfileStru fileStru;
        fileStru.filePath = file.fileName();
        fileStru.fileSize = file.size();
        fileStru.bExist = false;

        QStringList fileNameSplitList = filePath.split(".");
        QString fileSuffix = fileNameSplitList.last();
        if (fileSuffix.compare("PPT", Qt::CaseInsensitive) == 0 || fileSuffix.compare("pptx", Qt::CaseInsensitive) == 0)
        {
            fileStru.md5Str = KFileTransferCacheManage::getFileMd5(file);
        }
        else
        {
            fileStru.md5Str = "";
        }



        QList<checkfileStru> fileList;
        fileList.append(fileStru);
        mytcpsocket->isExistFile(fileList);

        for (checkfileStru fileInfo : fileList)
        {
            qDebug() << "fileName:" << fileInfo.filePath << (fileInfo.bExist ? " 服务器该文件存在 " : " 服务器该文件不存在 ");
        }
    }
}
