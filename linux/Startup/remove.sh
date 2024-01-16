#!/bin/bash

sudo systemctl stop era.service
sudo systemctl disable era.service
sudo rm -r /etc/systemd/system/era.service
