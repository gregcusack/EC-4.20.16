#!/bin/bash
# This script should disable all logical cpus (i.e. but not physical cpus)
for i in {20..39}; do
   echo "Disabling logical HT core $i."
   echo 0 > /sys/devices/system/cpu/cpu${i}/online;
done
