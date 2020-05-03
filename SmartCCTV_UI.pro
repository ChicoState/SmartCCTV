QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    sources/camera.cpp \
    sources/camera_daemon.cpp \
    sources/high_level_cctv_daemon_apis.cpp \
    sources/low_level_cctv_daemon_apis.cpp \
    sources/humanFilter.cpp \
    sources/motionFilter.cpp \
    sources/main.cpp \
    sources/mainwindow.cpp \
    sources/write_message.cpp

HEADERS += \
    sources/camera.hpp \
    sources/camera_daemon.h \
    sources/high_level_cctv_daemon_apis.h \
    sources/low_level_cctv_daemon_apis.h \
    sources/humanFilter.hpp \
    sources/motionFilter.hpp \
    sources/mainwindow.h \
    sources/write_message.h

FORMS += \
    sources/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
