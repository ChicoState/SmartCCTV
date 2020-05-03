#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <string>       /* for std::string */
#include <syslog.h>     /* for openlog(), syslog(), closelog() */
#include <cstdlib>      /* for getenv(), atexit(), exit(), EXIT_FAILURE */
#include <cstring>      /* for memset(), strcmp(), strncmp() */
#include <signal.h>     /* for sigaction(), sigemptyset(), SIGCONT */
#include <mqueue.h>     /* for mq_notify(), mq_receive(), mq_open(), mq_close(), mq_unlink() */
#include <unistd.h>     /* for sleep() */
#include <stdio.h>      /* for sprintf() */
#include<iostream>      /* for is_open(), close(), ifstream */
#include <fstream>      /* for is_open(), close(), ifstream */
#include <QPixmap>
#include <QtDebug>
#include <QLabel>

using namespace std;

mqd_t message_handler;  // message queue file descriptor
string message_handler_name = "/SmartCCTV_Message_handler";
QLabel* message_label = nullptr;
QLabel* daemon_label = nullptr;


void close_message_handler()
{
    syslog(log_facility | LOG_NOTICE, "Closing %s", message_handler_name.c_str());
    mq_close(message_handler);
    mq_unlink(message_handler_name.c_str());
}


void read_message(int)
{
    // Set up the message handler to recieve signal whenever a message comes in.
    // This is being done inside of the handler function because it has to be "reset" each time it is used.
    struct sigevent message_recieved;
    message_recieved.sigev_notify = SIGEV_SIGNAL;
    message_recieved.sigev_signo  = SIGCONT;
    mq_notify(message_handler, &message_recieved);

    // read a message
    int err;
    char message[121];
    memset(message, 0, 121);  // zero out the buffer
    // This is a while loop to handle when there are multiple messages queued up,
    // waiting to be read.
    while ( (err = mq_receive(message_handler, message, 120, nullptr)) != -1) {
        syslog(log_facility | LOG_NOTICE, "%s", message);
        // set the message into the label.
        if (message_label != nullptr) {
            message_label->setText(message);
        }

        if (strcmp("SmartCCTV encountered an error.", message) == 0) {
            if (daemon_label != nullptr) {
                daemon_label->setText("SmartCCTV have stopped running.");
            }
        } else if (strncmp("SmartCCTV failed to open ", message, 25) == 0) {
            if (daemon_label != nullptr) {
                daemon_label->setText("SmartCCTV have stopped running.");
            }
        } else if (strncmp("SmartCCTV could not create ", message, 27) == 0) {
            if (daemon_label != nullptr) {
                daemon_label->setText("Can not run SmartCCTV due to permission error.");
            }
        }

        memset(message, 0, 121);  // zero out the buffer
    }
}

bool chkList(string str, int dayAmt) 
{
	// Append the user's input date
	str += ".out";
	
	// Get correct starting day for iteration
	int startDay = stoi((str.substr(0, 1)[0] == '0' ? str.substr(1, 1):str.substr(0, 2)));
	for(int i = startDay; i <= startDay + dayAmt; i++) 
	{
		// Get current day
		char buf[3];
		sprintf (buf, "%02d", i);
		
		// Get complete filename using buf and user input
		string cFName = string(buf) + str.substr(2, 12);
		
		// Check that file is reachable and readable before closing
		ifstream curFile(cFName);
		if(!curFile.is_open()) 
		{
			return false;
		}
		curFile.close();
	}
	return true;
}

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

    // Set the QLabel to display the message that the daemon sends it.
    message_label = ui->label_3;
    daemon_label = ui->daemon_label;

    // Setup the message handler to recieve error messages from the daemon and dispaly them onto the GUI.
    // Make sure we can handle the SIGCONT message when the message queue notification sens the signal.
    struct sigaction action_to_take;
    // handle with this function
    action_to_take.sa_handler = read_message;
    // zero out the mask (allow any signal to interrupt)
    sigemptyset(&action_to_take.sa_mask);
    action_to_take.sa_flags = 0;
    // Setup the handler for SIGCONT.
    sigaction(SIGCONT, &action_to_take, nullptr);

    struct mq_attr mq_attributes;
    mq_attributes.mq_flags = 0;
    mq_attributes.mq_maxmsg = 5;
    mq_attributes.mq_msgsize = 120;

    if ( (message_handler = mq_open(message_handler_name.c_str(), O_RDWR | O_CREAT | O_EXCL | O_NONBLOCK, S_IRUSR | S_IWUSR, &mq_attributes)) == -1)
    {
        syslog(log_facility | LOG_ERR, "Error: Could not create the message handler %s %m", message_handler_name.c_str());
        syslog(log_facility | LOG_CRIT, "Failure starting the SmartCCTV application.");
        exit(EXIT_FAILURE);
    }

    // Set up the message handler to recieve signal whenever a message comes in.
    struct sigevent message_recieved;
    message_recieved.sigev_notify = SIGEV_SIGNAL;
    message_recieved.sigev_signo  = SIGCONT;
    mq_notify(message_handler, &message_recieved);

    syslog(log_facility | LOG_NOTICE, "Creating %s", message_handler_name.c_str());

    daemon_facade.set_daemon_info(home_directory);
    QDate date = QDate::currentDate();
    ui->dateEdit->setDate(date);
    ui->dateEdit->setMaximumDate(date);
    ui->dateEdit->setMinimumDate(date.addMonths(-6));

    syslog(log_facility | LOG_NOTICE, "SmartCCTV GUI was started.");
}


MainWindow::~MainWindow()
{
    syslog(log_facility | LOG_NOTICE, "The GUI window was closed.");
    close_message_handler();

    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
//    QString imgPath = home_directory;
//    imgPath.append("/Desktop/SmartCCTV/Charts/" + ui->dateEdit->date().toString("ddMMyyyy") + ".png");
//    qDebug() << ui->dateEdit->date().addDays(-1).toString("ddMMyyyy");
    int position = ui->horizontalSlider->value();
    qDebug() << position;
    QString arg;
    for(int i = position; i >= 0; i--){
        arg.append(ui->dateEdit->date().addDays(-i).toString("dd.MM.yyyy") + ".out ");
    }
    string utf8_text = arg.toUtf8().constData();
    qDebug() << arg;
    cout << utf8_text;
//    QPixmap img(imgPath);
//    ui->label->setPixmap(img.scaled(ui->label->width(),ui->label->height(),Qt::KeepAspectRatio));
}


void MainWindow::on_pushButton_Run_clicked()
{
    // Making a command to run or kill the daemon should reset the dispalyed error message.
    if (message_label != nullptr) {
        message_label->setText("");
    }

    int daemon = daemon_facade.run_daemon();
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
    // Making a command to run or kill the daemon should reset the dispalyed error message.
    if (message_label != nullptr) {
        message_label->setText("");
    }

    bool daemon = daemon_facade.kill_daemon();
    if(daemon == false){
        ui->daemon_label->setText("SmartCCTV is currently not running.");
    }
    else{
        ui->daemon_label->setText("SmartCCTV have stopped running.");
    }
}


void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
    if (position > 1){
        ui->day_label->setText("Additional Days");
    }
    else{
        ui->day_label->setText("Additional Day");
    }
    ui->range_counter->setText(QString::number(position));
}


void MainWindow::on_pushButton_2_clicked()
{
    syslog(log_facility | LOG_NOTICE, "The live stream viewer was started.");
    ui->label_3->setText("label_3 is used for printing diagnostic messages from the daemon");
}

