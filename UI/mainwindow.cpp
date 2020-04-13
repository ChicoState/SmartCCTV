#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "/home/weitaoli/Desktop/SmartCCTV/high_level_cctv_daemon_apis.h"
#include <QPixmap>
using namespace std;

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

void MainWindow::on_pushButton_clicked()
{
    QString imgPath = "/home/weitaoli/Desktop/SmartCCTV/Charts/" + ui->dateEdit->date().toString("ddMMyyyy") + ".png";
    QPixmap img(imgPath);
    ui->label->setPixmap(img.scaled(ui->label->width(),ui->label->height(),Qt::KeepAspectRatio));
}

void MainWindow::on_pushButton_Run_clicked()
{
    int daemon = run_daemon();
    if(daemon == 0){
        ui->daemon_label->setText("SmartCCTV is now running.");
    }
    else if(daemon == 1){
        ui->daemon_label->setText("SmartCCTV is already running.");
    }
    else if(daemon == 2){
        ui->daemon_label->setText("Can not run SmartCCTV due to permission error.");
    }
}

void MainWindow::on_pushButton_Kill_clicked()
{
    bool daemon = kill_daemon();
    if(daemon == false){
        ui->daemon_label->setText("SmartCCTV is currently not running.");
    }
    else{
        ui->daemon_label->setText("SmartCCTV have stopped running.");
    }
}
