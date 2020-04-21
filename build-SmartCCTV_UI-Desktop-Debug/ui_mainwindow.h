/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QDate>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QTabWidget *tabWidget;
    QWidget *tab;
    QPushButton *pushButton_Run;
    QPushButton *pushButton_Kill;
    QLabel *daemon_label;
    QPushButton *pushButton_2;
    QLabel *label_3;
    QWidget *tab_2;
    QLabel *label;
    QLabel *label_2;
    QPushButton *pushButton;
    QDateEdit *dateEdit;
    QLabel *range_counter;
    QSlider *horizontalSlider;
    QLabel *day_label;
    QMenuBar *menubar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(750, 400);
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        sizePolicy.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy);
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setGeometry(QRect(0, 0, 750, 400));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy1);
        tabWidget->setMinimumSize(QSize(500, 300));
        tabWidget->setTabPosition(QTabWidget::North);
        tabWidget->setTabShape(QTabWidget::Rounded);
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        sizePolicy.setHeightForWidth(tab->sizePolicy().hasHeightForWidth());
        tab->setSizePolicy(sizePolicy);
        pushButton_Run = new QPushButton(tab);
        pushButton_Run->setObjectName(QStringLiteral("pushButton_Run"));
        pushButton_Run->setGeometry(QRect(280, 270, 181, 41));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(pushButton_Run->sizePolicy().hasHeightForWidth());
        pushButton_Run->setSizePolicy(sizePolicy2);
        pushButton_Kill = new QPushButton(tab);
        pushButton_Kill->setObjectName(QStringLiteral("pushButton_Kill"));
        pushButton_Kill->setEnabled(true);
        pushButton_Kill->setGeometry(QRect(520, 270, 181, 41));
        daemon_label = new QLabel(tab);
        daemon_label->setObjectName(QStringLiteral("daemon_label"));
        daemon_label->setGeometry(QRect(220, 320, 301, 21));
        daemon_label->setAlignment(Qt::AlignCenter);
        pushButton_2 = new QPushButton(tab);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(40, 270, 181, 41));
        label_3 = new QLabel(tab);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(40, 20, 661, 241));
        label_3->setFrameShape(QFrame::StyledPanel);
        label_3->setFrameShadow(QFrame::Plain);
        label_3->setMidLineWidth(0);
        label_3->setAlignment(Qt::AlignCenter);
        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        sizePolicy.setHeightForWidth(tab_2->sizePolicy().hasHeightForWidth());
        tab_2->setSizePolicy(sizePolicy);
        label = new QLabel(tab_2);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(19, 90, 701, 231));
        label_2 = new QLabel(tab_2);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setEnabled(true);
        label_2->setGeometry(QRect(20, 30, 61, 17));
        pushButton = new QPushButton(tab_2);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(470, 30, 161, 41));
        dateEdit = new QDateEdit(tab_2);
        dateEdit->setObjectName(QStringLiteral("dateEdit"));
        dateEdit->setGeometry(QRect(20, 50, 171, 21));
        dateEdit->setLayoutDirection(Qt::LeftToRight);
        dateEdit->setDateTime(QDateTime(QDate(2020, 4, 14), QTime(0, 0, 0)));
        dateEdit->setMinimumDateTime(QDateTime(QDate(2020, 3, 1), QTime(0, 0, 0)));
        dateEdit->setMaximumDate(QDate(2020, 12, 31));
        dateEdit->setCalendarPopup(true);
        dateEdit->setTimeSpec(Qt::LocalTime);
        range_counter = new QLabel(tab_2);
        range_counter->setObjectName(QStringLiteral("range_counter"));
        range_counter->setEnabled(true);
        range_counter->setGeometry(QRect(300, 30, 21, 17));
        range_counter->setAlignment(Qt::AlignCenter);
        horizontalSlider = new QSlider(tab_2);
        horizontalSlider->setObjectName(QStringLiteral("horizontalSlider"));
        horizontalSlider->setGeometry(QRect(250, 50, 160, 21));
        horizontalSlider->setMinimum(1);
        horizontalSlider->setMaximum(7);
        horizontalSlider->setOrientation(Qt::Horizontal);
        horizontalSlider->setTickPosition(QSlider::NoTicks);
        day_label = new QLabel(tab_2);
        day_label->setObjectName(QStringLiteral("day_label"));
        day_label->setGeometry(QRect(320, 30, 67, 17));
        tabWidget->addTab(tab_2, QString());
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 750, 22));
        MainWindow->setMenuBar(menubar);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        pushButton_Run->setText(QApplication::translate("MainWindow", "Run", Q_NULLPTR));
        pushButton_Kill->setText(QApplication::translate("MainWindow", "Kill", Q_NULLPTR));
        daemon_label->setText(QString());
        pushButton_2->setText(QApplication::translate("MainWindow", "All Cameras", Q_NULLPTR));
        label_3->setText(QApplication::translate("MainWindow", "SCREEN FOR LIVE VIEW OF CAMERA", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainWindow", "Home", Q_NULLPTR));
        label->setText(QString());
        label_2->setText(QApplication::translate("MainWindow", "Date:", Q_NULLPTR));
        pushButton->setText(QApplication::translate("MainWindow", "Find", Q_NULLPTR));
        dateEdit->setDisplayFormat(QApplication::translate("MainWindow", "M/d/yyyy", Q_NULLPTR));
        range_counter->setText(QApplication::translate("MainWindow", "1", Q_NULLPTR));
        day_label->setText(QApplication::translate("MainWindow", "Day", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainWindow", "Statistics", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
