#!/usr/bin/env python3

import os, time, sys
import json
import argparse
import requests
import RPi.GPIO as GPIO
import paho.mqtt.client as mqtt

from PnP import ERaPnPPi as PnP

ERA_BOARD_ID = ""
ERA_BOARD_TYPE = "Raspberry"
ERA_MODEL_TYPE = "ERa"

ERA_FIRMWARE_MAJOR = 0
ERA_FIRMWARE_MINOR = 9
ERA_FIRMWARE_REVISION = 0
ERA_FIRMWARE_VERSION = "0.9.0"

BUTTON_GPIO = 16

parser = argparse.ArgumentParser(description="ERa Options.")
parser.add_argument("-l", "--loc", dest="location", default="SG",
                    help="Location host (default: SG)")

args = parser.parse_args()

try:
    with open("config.json") as json_file:
        config = json.load(json_file)
    to_save = False
except:
    config = PnP.provision(args.location, ERA_BOARD_TYPE,
                            ERA_MODEL_TYPE, ERA_FIRMWARE_VERSION)
    to_save = True

def reset_config():
    if os.path.exists("./era"):
        os.remove("./era")
    if os.path.exists("config.json"):
        print("Resetting configuration")
        os.remove("config.json")
        # Restart
        os.execv(sys.executable, ['python3'] + sys.argv)
        sys.exit(0)

def button_callback(channel):
    if GPIO.input(channel) == 1:
        return

    print("Hold button for 10 seconds to reset configuration")
    start_time = time.time()
    # Wait for the button up
    while (GPIO.input(channel) == 0 and
           time.time() - start_time <= 10):
        time.sleep(0.1)
    if time.time() - start_time > 10:
        reset_config()

def on_connect(client, userdata, flags, rc):
    if (rc == 0):
        client.connected_flag = True
        if to_save:
            with open("config.json", "w") as json_file:
                json.dump(config, json_file)
            print("Configuration is saved")
        print("Verify connect OK!")
    else:
        print("Verify connect FAIL, code =", rc)
        

def verify_connect():
    if not to_save:
        return
    mqtt.Client.connected_flag = False
    client = mqtt.Client(client_id = config["auth"], clean_session = True)
    client.username_pw_set(config["auth"], config["auth"])
    client.on_connect = on_connect
    client.connect(config["host"], config["port"])

    # Wait verify
    client.loop_start()
    start_time = time.time()
    while (not client.connected_flag and
           time.time() - start_time <= 20):
        time.sleep(1)
    client.loop_stop()
    client.disconnect()
    if not client.connected_flag:
        # Restart
        os.execv(sys.executable, ['python3'] + sys.argv)
        sys.exit(0)

def update_ota():
    url = ("https://backend.eoh.io/api/chip_manager"
           "/firmware?code={0}&firm_version={1}.{2}&board={3}").format(
            config["auth"],
            ERA_FIRMWARE_MAJOR,
            ERA_FIRMWARE_MINOR,
            ERA_MODEL_TYPE
        )
    print("Firmware update URL: {0}".format(url))
    response = requests.get(url)
    if (response.status_code != 200):
        return
    descriptor = os.open(path = "./era",
                         flags = (
                            os.O_WRONLY |
                            os.O_CREAT |
                            os.O_TRUNC
                        ),
                        mode = 0o755)
    with open(descriptor, "wb") as file:
        file.write(response.content)
    print("Update successfully!")


GPIO.setmode(GPIO.BCM)
GPIO.setup(BUTTON_GPIO, GPIO.IN, pull_up_down = GPIO.PUD_UP)

GPIO.add_event_detect(BUTTON_GPIO, GPIO.BOTH,
                    callback = button_callback,
                    bouncetime = 50)

verify_connect()

while True:
    if os.path.exists("./era"):
        cmd = "sudo ./era --token={0} --host={1} --id={2}".format(config["auth"], config["host"], ERA_BOARD_ID)
        rt = os.system(cmd)
        rt = (rt >> 8) & 0xFF
        if rt == 27:
            update_ota()
        elif rt == 2:
            os.remove("./era")
    else:
        cmd = "make clean all target=raspberry location={0}".format(args.location)
        os.system(cmd)