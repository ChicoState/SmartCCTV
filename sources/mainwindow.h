/**
 * File Name:  mainwindow.h
 * Created By:  Wei Tao Li <>
 * Created On:  
 *
 * Modified By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Modified On:  5/16/20
 *
 * Description:
 * This file contains the definition the MainWindow class.
 * This is basically the class that handles all input and output from the GUI.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "high_level_cctv_daemon_apis.h"
#include "livestream_facade.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_Run_clicked();

    void on_pushButton_Kill_clicked();

    void on_horizontalSlider_sliderMoved(int position);

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    Daemon_facade daemon_facade;
    LiveStream_facade liveStream_facade;
    const char* home_directory;
};

#endif // MAINWINDOW_H
