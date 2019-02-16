#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mytcpsocket = KFileTransferRecevicer::GetInstance();
}

MainWindow::~MainWindow()
{
    delete ui;
}

