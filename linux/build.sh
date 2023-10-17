#!/bin/bash

case "$1" in
raspberry)
    sudo apt-get update
    sudo apt-get upgrade
    sudo apt-get install build-essential git-core
    git clone git://git.drogon.net/wiringPi
    cd wiringPi
    git pull origin
    ./build
    gpio -v
    cd ..
    make clean all target=raspberry
    exit 0
    ;;
tinker)
    sudo apt-get update
    sudo apt-get upgrade
    sudo apt-get install build-essential git-core
    git clone https://github.com/TinkerBoard/gpio_lib_c.git
    cd gpio_lib_c
    git pull origin
    chmod a+x build
    sudo ./build
    gpio -v
    cd ..
    make clean all target=tinker
    exit 0
    ;;
orangepi)
    sudo apt-get update
    sudo apt-get upgrade
    sudo apt-get install build-essential git-core
    git clone https://github.com/orangepi-xunlong/wiringOP.git
    cd wiringOP
    git pull origin
    ./build
    gpio -v
    cd ..
    make clean all target=orangepi
    exit 0
    ;;
linux)
    make clean all
    exit 0
    ;;
esac

echo "Please specify platform: raspberry, tinker, orangepi, linux"
exit 1