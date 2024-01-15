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
    cp *.a ../../MbedTLS
    cp -R ../include/* ../../MbedTLS
    cd ../../
    rm mbedtls-3.3.0.tar.gz
    rm -r mbedtls-3.3.0
    echo "Mbed-TLS has been successfully installed with version mbedtls-3.3.0."
    exec bash
    # exit 0
    ;;
mbedtls_latest)
    latest_version=$(curl -s https://api.github.com/repos/Mbed-TLS/mbedtls/releases/latest | grep -oP '"tag_name": "\K(.*)(?=")')
    if [ -z "$latest_version" ]; then
        echo "Failed to retrieve the latest version of mbedtls. Please check your internet connection."
        exec bash
        # exit 1
    fi
    download_url="https://github.com/Mbed-TLS/mbedtls/archive/refs/tags/${latest_version}.tar.gz"
    wget "$download_url" -O "mbedtls-${latest_version}.tar.gz"
    tar -xzvf "mbedtls-${latest_version}.tar.gz"
    source_dir=$(tar -tzf "mbedtls-${latest_version}.tar.gz" | head -1 | cut -f1 -d'/')
    cd "$source_dir"
    python3 -m pip install -r scripts/basic.requirements.txt
    cd library
    make all
    cp *.a ../../MbedTLS
    cp -R ../include/* ../../MbedTLS
    cd ../../
    rm "mbedtls-${latest_version}.tar.gz"
    rm -r "$source_dir"
    echo "Mbed-TLS has been successfully updated to version ${latest_version}."
    exec bash
    # exit 0
    ;;
esac

echo "Please specify tls library: mbedtls, mbedtls_latest"
exec bash
# exit 1