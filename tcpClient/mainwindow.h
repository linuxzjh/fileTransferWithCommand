#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "KFileTransferSender.h"
#include <QFileDialog>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_btn_connect_clicked();
    void on_btn_selectFile_clicked();
    void on_btn_upFile_clicked();
    void on_btn_cancel_clicked();

    void onSendFileSize(qint64);
    void onRecvSize(qint64);
private:
    Ui::MainWindow *ui;
    KFileTransferSender *mytcpsocket;
};

#endif // MAINWINDOW_H
