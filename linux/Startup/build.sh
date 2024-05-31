#!/bin/bash

source_folder=""
service_src="era.service"
service_dst="era.service"

case "$1" in
raspberry|tinker)
    source_folder="$1"
    ;;
*)
    exec bash
    ;;
esac

sudo cp "./${source_folder}/${service_src}" "/etc/systemd/system/${service_dst}"
sudo systemctl enable "${service_dst}"
sudo systemctl start "${service_dst}"
