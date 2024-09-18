# Raspberry Pi, Tinker Board, Linux Guide

Step 1: Install Raspbian OS in your Raspberry Pi.

Step 2: Connect your Raspberry Pi to the internet.

Step 3: Install WiringPi library:
    https://github.com/WiringPi/WiringPi.git

Step 4: Download library and build ERa:
```bash
$ git clone https://github.com/eoh-jsc/era-lib.git
$ cd era-lib/linux
$ make clean all target=raspberry
```
Enable TLS (Raspbian 32-bit)
```bash
$ make clean all target=raspberry tls=true
```

Step 5: Run ERa with your token:
```bash
$ sudo ./era --token=YourAuthToken
```
or
```bash
$ sudo ./era --token=YourAuthToken --id=YourBoardID
```
