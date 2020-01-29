#!/bin/bash

dyn_pid=$1
bash_pid=$2

filename="/home/greg/Desktop/plots/${dyn_pid}_${bash_pid}_cpu_util.csv"

top -bn 200 -d .5 | grep -E -w "$dyn_pid|$bash_pid" | awk '{print $9}' > "$filename"

#while true
#do
#	cpu_dyn=$(top -bn 1 -d .1 | grep -w -E "^ *$dyn_pid" | awk '{print $9}')
#
#	cpu_bash=$(top -bn 1 -d .1 | grep -w -E "^ *$bash_pid" | awk '{print $9}')
#	echo "$cpu_dyn,$cpu_bash" >> "$filename"
#done
