#!/bin/sh
mkdir -p /etc/sv/mouse-remap
cp mouse-remap /etc/sv/mouse-remap/run
ln -s /etc/sv/mouse-remap /var/service/
