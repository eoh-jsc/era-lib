#!/bin/bash

case "$1" in
mbedtls)
    wget https://github.com/Mbed-TLS/mbedtls/archive/refs/tags/v3.3.0.tar.gz \
    -O mbedtls-3.3.0.tar.gz
    tar -xzvf mbedtls-3.3.0.tar.gz
    cd mbedtls-3.3.0
    python3 -m pip install -r scripts/basic.requirements.txt
    cd library
    make all
    cp *.a ../../Mbedtls
    cd ../../
    rm mbedtls-3.3.0.tar.gz
    rm -r mbedtls-3.3.0
    exit 0
    ;;
esac

echo "Please specify tls library: mbedtls"
exit 1