#include "mainwindow.h"
#include "ui_mainwindow.h"
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
