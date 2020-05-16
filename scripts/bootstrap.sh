#!/bin/bash

# Generate the qmake makefile
qmake SmartCCTV_UI.pro
# Rename the qmake makefile
mv Makefile Makefile_qmake
# Rename/copy the skeleton makefile, set it as the real makefile,
# having a backup of the original copy.
cp Makefile_skeleton Makefile

# Swap those stored line variables into the skeleton with sed()
# by looking for lines we know will show up in the skeleton
old_incpath=$(cat Makefile_skeleton | grep "INCPATH       = ")
old_libs=$(cat Makefile_skeleton | grep "LIBS          = ")
old_moc_mainwindow=$(sed -n '/moc_mainwindow.cpp:/,/^$/p' Makefile_skeleton)
old_ui_mainwindow=$(sed -n '/ui_mainwindow.h:/,/^$/p' Makefile_skeleton)

# Grep the info that's in the generated Makefile 
# and store it into variables
new_incpath=$(cat Makefile_qmake | grep "INCPATH       = ")
new_libs=$(cat Makefile_qmake | grep "LIBS          = ")
new_moc_mainwindow=$(sed -n '/moc_mainwindow.cpp:/,/^$/p' Makefile_qmake)
new_ui_mainwindow=$(sed -n '/ui_mainwindow.h:/,/^$/p' Makefile_qmake)

replace "$old_incpath" "$new_incpath" -- Makefile
replace "$old_libs" "$new_libs" -- Makefile
replace "$old_moc_mainwindow" "$new_moc_mainwindow" -- Makefile
replace "$old_ui_mainwindow" "$new_ui_mainwindow" -- Makefile

# Remove the temporary Makefile_qmake
rm Makefile_qmake
# Now you just have the system dependent Makefile
# and the Makefile_skeleton is unchanged
