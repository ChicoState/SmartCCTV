#!/bin/bash

# Copy data from the syslog file to
# a new copy of a local log file for 
# that specific day.
cp /var/log/syslog ~/SCCTV/"$(date)".log
echo "" > /var/log/syslog
