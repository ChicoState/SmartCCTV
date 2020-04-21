#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../high_level_cctv_daemon_apis.h"

#include <syslog.h>     /* for openlog(), syslog(), closelog() */
#include <cstdlib>      /* for getenv(), atexit(), exit(), EXIT_FAILURE */
#include <QPixmap>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), home_directory(nullptr)
{
    ui->setupUi(this);
    // The SmartCCTv GUI also writes messages to the syslog, so we need to open that as well.
    openlog("SmartCCTV_GUI", LOG_PID, log_facility);
    atexit(closelog);

    home_directory = getenv("HOME");
    if (home_directory == nullptr) {
        syslog(log_facility | LOG_ERR, "Error: $HOME environmental varaible not set : failed to identify home directory");
        syslog(log_facility | LOG_CRIT, "Failure starting the SmartCCTV application.");
        exit(EXIT_FAILURE);
    }

    set_daemon_info(home_directory);
}

MainWindow::~MainWindow()
{
    syslog(log_facility | LOG_NOTICE, "The GUI window was closed.");
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString imgPath = home_directory;
    imgPath.append("/Desktop/SmartCCTV/Charts/" + ui->dateEdit->date().toString("ddMMyyyy") + ".png");
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

