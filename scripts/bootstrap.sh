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
e=$(sed -n '/INCPATH       = /,/^$/p' Makefile_qmake)
f=$(sed -n '/LIBS          = /,/^$/p' Makefile_qmake)
g=$(sed -n '/moc_mainwindow.cpp: /,/^$/p' Makefile_qmake)
h=$(sed -n '/ui_mainwindow.h: /,/^$/p' Makefile_qmake)

# Swap those stored line variables into the skeleton with sed()
# by looking for lines we know will show up in the skeleton
a=$(sed -n '/INCPATH       = /,/^$/p' Makefile_skeleton)
b=$(sed -n '/LIBS          = /,/^$/p' Makefile_skeleton)
c=$(sed -n '/moc_mainwindow.cpp: /,/^$/p' Makefile_skeleton)
d=$(sed -n '/ui_mainwindow.h: /,/^$/p' Makefile_skeleton)

replacement1="s/'$(echo $a)'/'$(echo $e)'/"
replacement2="s/'$(echo $b)'/'$(echo $f)'/"
replacement3="s/'$(echo $c)'/'$(echo $g)'/"
replacement4="s/'$(echo $d)'/'$(echo $h)'/"

# Perform the actual repalcements
sed -i $replacement1 Makefile
#sed -i $replacement2 Makefile
#sed -i $replacement3 Makefile
#sed -i $replacement4 Makefile

#sed -i 's|' "${a}" '|' "${e}" '|' Makefile
#sed -i 's|'"${b}"'|'"${f}"'|' Makefile
#sed -i 's|'"${c}"'|'"${g}"'|' Makefile
#sed -i 's|'"${d}"'|'"${h}"'|' Makefile

# Remove the temporary Makefile_qmake
rm Makefile_qmake
# Now you just have the system dependent Makefile
# and the Makefile_skeleton is unchanged
