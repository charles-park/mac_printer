#!/bin/bash

#--------------------------
# delay for system stability
#--------------------------
sleep 10 && sync

#--------------------------
# ODROID-M1S Server app
#--------------------------
/root/mac_printer/mac_printer > /dev/null 2>&1
