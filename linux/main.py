#!/usr/bin/env python3

import os, time, sys
import json
import hashlib
import argparse
import requests
import RPi.GPIO as GPIO
import paho.mqtt.client as mqtt

from PnP import ERaPnPPi as PnP

ERA_BOARD_ID = ""
ERA_BOARD_TYPE = "Raspberry"
ERA_MODEL_TYPE = "ERa"
ERA_PNP_ORG = "EoH"
ERA_PNP_PREFIX = "ERa"
ERA_PNP_MODE = "UDP"

ERA_MAJOR = 1
ERA_MINOR = 4
ERA_PATCH = 2
ERA_VERSION = "1.4.2"
ERA_FIRMWARE_VERSION = "1.4.2"

BUTTON_GPIO = 16

parser = argparse.ArgumentParser(description="ERa Options.")
parser.add_argument("-l", "--loc", dest="location", default="VN",
                    help="Location host (default: VN)")
parser.add_argument("-t", "--token", dest="token", default="",
                    help="Your auth token")
parser.add_argument("-a", "--host", dest="host", default="",
                    help="Server name")
parser.add_argument("-b", "--button", dest="button", default=BUTTON_GPIO,
                    type=int, help="Button pin")

args = parser.parse_args()
config = dict()

try:
    with open("config.json") as json_file:
        config = json.load(json_file)
    to_save = False
except:
    if args.token:
        config["auth"] = args.token
        if args.host:
            config["host"] = args.host
        elif args.location == "SG":
            config["host"] = PnP.ERA_MQTT_HOST_SG
        else:
            config["host"] = PnP.ERA_MQTT_HOST_VN
        to_save = False
    else:
        config = PnP.provision(args.location, ERA_BOARD_TYPE,
                            ERA_MODEL_TYPE, ERA_VERSION, ERA_FIRMWARE_VERSION,
                            ERA_PNP_ORG, ERA_PNP_PREFIX, ERA_PNP_MODE)
        to_save = True

def reset_config():
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

def md5_calculate(filename):
    hash_md5 = hashlib.md5()
    with open(filename, "rb") as file:
        for chunk in iter(lambda: file.read(2 ** 20), b""):
            hash_md5.update(chunk)
    return hash_md5.hexdigest()

def update_ota():
    ota_config = dict()
    try:
        with open("./ota.txt") as json_file:
            ota_config = json.load(json_file)
    except:
        return
    if not "url" in ota_config:
        return
    print("Firmware update URL: {0}".format(ota_config["url"]))
    response = requests.get(ota_config["url"])
    if (response.status_code != 200):
        return
    descriptor = os.open(path = "./era.bin",
                         flags = (
                            os.O_WRONLY |
                            os.O_CREAT |
                            os.O_TRUNC
                        ),
                        mode = 0o755)
    with open(descriptor, "wb") as file:
        file.write(response.content)
    if "hash" in ota_config:
        md5_hash = md5_calculate("./era.bin");
        if ota_config["hash"] == md5_hash:
            print("Verify match: {0}".format(ota_config["hash"]))
        else:
            print("No MD5 match: Local = {0}, Target = {1}".format(md5_hash, ota_config["hash"]))
            return
    if os.path.exists("./era"):
        os.remove("./era")
    os.rename("./era.bin", "./era")
    print("Update successfully!")

GPIO.setmode(GPIO.BCM)
GPIO.setup(args.button, GPIO.IN, pull_up_down = GPIO.PUD_UP)

GPIO.add_event_detect(args.button, GPIO.BOTH,
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