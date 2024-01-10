#!/bin/bash
systemctl disable odroid-mac_printer.service && sync

cp ./odroid-mac_printer.service /etc/systemd/system/ && sync

systemctl enable odroid-mac_printer.service && sync

systemctl restart odroid-mac_printer.service && sync
