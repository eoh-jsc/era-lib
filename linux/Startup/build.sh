#!/bin/bash

source_folder=""
service_file="era.service"

case "$1" in
raspberry)
    source_folder="raspberry"
    ;;
tinker)
    source_folder="tinker"
    ;;
*)
    exec bash
    ;;
esac

sudo cp ./${source_folder}/${service_file} /etc/systemd/system/${service_file}
sudo systemctl enable ${service_file}
sudo systemctl start ${service_file}
