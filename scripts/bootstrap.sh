#!/bin/bash

# Generate the qmake makefile
qmake SmartCCTV_UI.pro
# Rename the qmake makefile
mv Makefile Makefile_qmake
# Rename/copy the skeleton makefile, set it as the real makefile,
# having a backup of the original copy.
cp Makefile_skeleton Makefile

# Grep the info that's in the generated Makefile 
# and store it into variables
e=$(cat Makefile_qmake | grep "INCPATH       = ")
f=$(cat Makefile_qmake | grep "LIBS          = ")
g=$(cat Makefile_qmake | grep "moc_mainwindow.cpp: ")
h=$(cat Makefile_qmake | grep "ui_mainwindow.h: ")

# Swap those stored line variables into the skeleton with sed()
# by looking for lines we know will show up in the skeleton
a="INCPATH       = -I. -isystem /usr/include/x86_64-linux-gnu/qt5 -isystem /usr/include/x86_64-linux-gnu/qt5/QtWidgets -isystem /usr/include/x86_64-linux-gnu/qt5/QtGui -isystem /usr/include/x86_64-linux-gnu/qt5/QtCore -I. -I. -I/usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++-64"
b="LIBS          = $(SUBLIBS) -L/usr/X11R6/lib64 -lQt5Widgets -lQt5Gui -lQt5Core -lGL -lpthread"
c="moc_mainwindow.cpp: sources/high_level_cctv_daemon_apis.h \ sources/mainwindow.h /usr/lib/x86_64-linux-gnu/qt5/bin/moc $(DEFINES) -I/usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++-64 -I/home/konstantin/Documents/programming/SmartCCTV -I/usr/include/x86_64-linux-gnu/qt5 -I/usr/include/x86_64-linux-gnu/qt5/QtWidgets -I/usr/include/x86_64-linux-gnu/qt5/QtGui -I/usr/include/x86_64-linux-gnu/qt5/QtCore -I/usr/include/c++/5 -I/usr/include/x86_64-linux-gnu/c++/5 -I/usr/include/c++/5/backward -I/usr/lib/gcc/x86_64-linux-gnu/5/include -I/usr/local/include -I/usr/lib/gcc/x86_64-linux-gnu/5/include-fixed -I/usr/include/x86_64-linux-gnu -I/usr/include sources/mainwindow.h -o moc_mainwindow.cpp"
d="i_mainwindow.h: sources/mainwindow.ui /usr/lib/x86_64-linux-gnu/qt5/bin/uic sources/mainwindow.ui -o ui_mainwindow.h"
sed -i 's|'"${a}"'|'"${e}"'|' Makefile
sed -i 's|'"${b}"'|'"${f}"'|' Makefile
sed -i 's|'"${c}"'|'"${g}"'|' Makefile
sed -i 's|'"${d}"'|'"${h}"'|' Makefile

# Remove the temporary Makefile_qmake
rm Makefile_qmake
# Now you just have the system dependent Makefile
# and the Makefile_skeleton is unchanged
