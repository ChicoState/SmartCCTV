#!/bin/bash

# Create a skeleton backup
cp ./../Makefile_skeleton ./../Makefile_skeleton_backup

# Grep the info that's in the generated Makefile 
# and store it into variables
e=$(cat ./../Makefile | grep "INCPATH       = ")
f=$(cat ./../Makefile | grep "LIBS          = ")
g=$(cat ./../Makefile | grep "moc_mainwindow.cpp: ")
h=$(cat ./../Makefile | grep "ui_mainwindow.h: ")

# Swap those stored line variables into the skeleton with sed()
# by looking for lines we know will show up in the skeleton
a="INCPATH       = -I. -isystem /usr/include/x86_64-linux-gnu/qt5 -isystem /usr/include/x86_64-linux-gnu/qt5/QtWidgets -isystem /usr/include/x86_64-linux-gnu/qt5/QtGui -isystem /usr/include/x86_64-linux-gnu/qt5/QtCore -I. -I. -I/usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++-64"
b="LIBS          = $(SUBLIBS) -L/usr/X11R6/lib64 -lQt5Widgets -lQt5Gui -lQt5Core -lGL -lpthread"
c="moc_mainwindow.cpp: sources/high_level_cctv_daemon_apis.h \ sources/mainwindow.h /usr/lib/x86_64-linux-gnu/qt5/bin/moc $(DEFINES) -I/usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++-64 -I/home/konstantin/Documents/programming/SmartCCTV -I/usr/include/x86_64-linux-gnu/qt5 -I/usr/include/x86_64-linux-gnu/qt5/QtWidgets -I/usr/include/x86_64-linux-gnu/qt5/QtGui -I/usr/include/x86_64-linux-gnu/qt5/QtCore -I/usr/include/c++/5 -I/usr/include/x86_64-linux-gnu/c++/5 -I/usr/include/c++/5/backward -I/usr/lib/gcc/x86_64-linux-gnu/5/include -I/usr/local/include -I/usr/lib/gcc/x86_64-linux-gnu/5/include-fixed -I/usr/include/x86_64-linux-gnu -I/usr/include sources/mainwindow.h -o moc_mainwindow.cpp"
d="i_mainwindow.h: sources/mainwindow.ui /usr/lib/x86_64-linux-gnu/qt5/bin/uic sources/mainwindow.ui -o ui_mainwindow.h"
sed -i 's|'"${a}"'|'"${e}"'|' Makefile_skeleton
sed -i 's|'"${b}"'|'"${f}"'|' Makefile_skeleton
sed -i 's|'"${c}"'|'"${g}"'|' Makefile_skeleton
sed -i 's|'"${d}"'|'"${h}"'|' Makefile_skeleton

# Remove the temporary Makefile and then rename the skeleton to
# become the actual Makefile
rm ./../Makefile
mv ./../Makefile_skeleton Makefile
