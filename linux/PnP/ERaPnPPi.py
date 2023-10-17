from http.server import BaseHTTPRequestHandler, HTTPServer
from urllib.parse import urlparse, parse_qsl
from contextlib import suppress
import socket, errno
import sys, time
import nmcli
import json

WIFI_SCAN_MAX = 15

WIFI_AP_PASS = "Eoh@2021"
ERA_MQTT_HOST_VN = "mqtt1.eoh.io"
ERA_MQTT_HOST_SG = "remarkable-accountant.cloudmqtt.com"
ERA_MQTT_PORT = 1883

def era_log(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

class WiFi:
    def __init__(self):
        wifi_devices = list(filter(lambda x: x.device_type == "wifi", nmcli.device()))
        assert(len(wifi_devices) > 0)
        self.device = wifi_devices[0].device
        details = nmcli.device.show(self.device)
        self.mac_addr = details["GENERAL.HWADDR"]
        self.ap_name = "ERa AP"
        era_log("WiFi devices:", wifi_devices)
        era_log("WiFi MAC:    ", self.mac_addr)

    def mac_address(self):
        return self.mac_addr

    def set_hostname(self, name):
        nmcli.general.set_hostname(name)
        era_log("Hostname:    ", name)

    def _cleanup(self, conn):
        with suppress(Exception):
            nmcli.connection.down(conn)
        with suppress(Exception):
            nmcli.connection.delete(conn)

    def create_ap(self, ssid, password):
        self.remove_ap()
        nmcli.connection.add(name = self.ap_name, conn_type = "wifi", options = {
            "ssid": ssid,
            "ipv4.method": "shared",
            "ipv4.addresses": "192.168.27.1/24",
            "802-11-wireless.mode": "ap",
            "802-11-wireless.band": "bg",
            "802-11-wireless-security.key-mgmt": "wpa-psk",
            "802-11-wireless-security.psk": password
        })
        nmcli.connection.up(self.ap_name)
        era_log("AP SSID:     ", ssid)

    def remove_ap(self):
        self._cleanup(self.ap_name)

    def scan(self):
        results = []
        for net in nmcli.device.wifi():
            signal = max(30, min(net.signal, 100))
            rssi_max = -20
            rssi_min = -90
            rssi = int(-((((rssi_max - rssi_min) * (signal - 100)) / -70) - rssi_max))
            results.append({
                "ssid":         net.ssid,
                "bssid":        net.bssid,
                "freq":         net.freq,
                "rssi":         rssi,
                "encryption":   net.security if len(net.security) else "OPEN",
                "channel":      net.chan
            })

        return results

    def scanCompact(self):
        results = {}
        wifi_infos = []
        nets = 0
        for net in nmcli.device.wifi():
            signal = max(30, min(net.signal, 100))
            rssi_max = -20
            rssi_min = -90
            rssi = int(-((((rssi_max - rssi_min) * (signal - 100)) / -70) - rssi_max))
            wifi_infos.append({
                "ssid":         net.ssid,
                "rssi":         rssi
            })
            nets += 1
            if nets >= WIFI_SCAN_MAX:
                break
        results["type"] = "data_wifi"
        results["wifi"] = wifi_infos
        return results

    def restart(self):
        nmcli.radio.wifi_off()
        nmcli.radio.wifi_on()

    def connect(self, ssid, password):
        try:
            self._cleanup(ssid)
            nmcli.device.wifi_connect(ssid, password)
        except Exception as e:
            era_log("Connect WiFi error: ", e)

class HTTPHandler(BaseHTTPRequestHandler):
    def _reply_json(self, data):
        self.send_response(200)
        self.send_header("Content-type", "application/json")
        self.end_headers()
        self.wfile.write(json.dumps(data).encode())

    def _parse_config(self, q):
        if not "token" in q:
            self.send_error(400)
            return
        q["auth"] = q.pop("token")
        if not "host" in q:
            q["host"] = self.server.era_host
        if not "port" in q:
            q["port"] = ERA_MQTT_PORT
        if not "username" in q:
            q["username"] = q["auth"]
        if not "password" in q:
            q["password"] = q["auth"]
        if "save" in q:
            self._reply_json({"status":"ok","message":"Configuration saved"})
        else:
            self._reply_json({"status":"ok","message":"Connecting wifi..."})
        self.server.era_config = q

    def log_message(self, format, *args):
        return

    def do_GET(self):
        o = urlparse(self.path)
        q = dict(parse_qsl(o.query))
        if o.path == "/api/board_info.json":
            self._reply_json(self.server.era_info)
        elif o.path == "/api/wifi_scan.json":
            self._reply_json(self.server.wifi_networks)
        elif o.path == "/api/config":
            self._parse_config(q)
        else:
            self.send_error(404)

    def do_POST(self):
        o = urlparse(self.path)
        if o.path == "/api/config":
            len = int(self.headers.get("Content-Length"))
            buf = self.rfile.read(len)
            q = json.loads(buf)
            self._parse_config(q)
        else:
            self.send_error(404)

class UDPHandle:
    def __init__(self, host, port):
        self.server = self
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        try:
            self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
        except AttributeError:
            pass
        try:
            self.sock.bind((host, port))
        except socket.error as e:
            if e.errno == errno.EADDRINUSE:
                era_log("Port is already in use")

    def _reply_json(self, data):
        if self.address:
            self.sock.sendto(json.dumps(data).encode(), self.address)

    def _parse_config(self, data):
        self._type_parse(data)
        # Update later

    def _type_parse(self, data):
        obj = json.loads(data)
        if not "type" or not "data" in obj:
            return
        if obj["type"] == "connect":
            self._connect_parse(obj["data"])
        elif obj["type"] == "scan":
            self._scan_parse(obj["data"])
        d = list(obj["data"])
        # Update later

    def _connect_parse(self, obj):
        if not "wifi" in obj:
            return
        q = obj["wifi"]
        if "token" in obj:
            q["auth"] = obj["token"]
        if "host" in obj:
            q["host"] = obj["host"]
        if "port" in obj:
            q["port"] = obj["port"]
        if "token" in q:
            q["auth"] = q.pop("token")
        if not "auth" in q:
            return
        if not "host" in q:
            q["host"] = self.server.era_host
        if not "port" in q:
            q["port"] = ERA_MQTT_PORT
        if not "username" in q:
            q["username"] = q["auth"]
        if not "password" in q:
            q["password"] = q["auth"]
        self._reply_json({"status":"ok","message":"Connecting wifi..."})
        time.sleep(1)
        self.server.era_info["token"] = q["auth"]
        self._reply_json(self.server.era_info)
        self.server.era_config = q

    def _scan_parse(self, obj):
        if not "wifi" in obj:
            return
        self._reply_json(self.server.wifi_networks)

    def do_RECV(self):
        data, self.address = self.sock.recvfrom(1024)
        return data.decode()

    def handle_request(self):
        buf = self.do_RECV()
        self._parse_config(buf)

    def __enter__(self):
        return self.server

    def __exit__(self, exc_type, exc_value, exc_traceback):
        self.sock.close()

def provision(location, board, model, ver, fw_ver, org = "EoH", prefix = "ERa", mode = "TCP"):
    wifi = WiFi()

    wifi_networks = {}
    if mode == "TCP":
        wifi_networks = wifi.scan()
    else:
        wifi_networks = wifi.scanCompact()

    suffix = wifi.mac_address().replace(":", "")
    my_ssid = org + "." + prefix + "." + suffix
    my_ssid = my_ssid.lower()
    my_imei = prefix.upper() + "-" + suffix.lower()

    config = None

    if mode == "TCP":
        try:
            wifi.create_ap(my_ssid, WIFI_AP_PASS)
            with HTTPServer(("0.0.0.0", 11080), HTTPHandler) as httpd:
                httpd.era_info = {
                    "board":            board,
                    "model":            model,
                    "imei":             my_imei,
                    "version":          ver,
                    "firmware_version": fw_ver,
                    "ssid":             my_ssid,
                    "pass":             WIFI_AP_PASS,
                    "bssid":            wifi.mac_address(),
                    "ip":               "192.168.27.1"
                }
                httpd.wifi_networks = wifi_networks
                httpd.era_config = None
                if location == "SG":
                    httpd.era_host = ERA_MQTT_HOST_SG
                else:
                    httpd.era_host = ERA_MQTT_HOST_VN

                era_log("Waiting for ERa App connection...")
                while httpd.era_config is None:
                    httpd.handle_request()
                config = httpd.era_config
        finally:
            wifi.remove_ap()
    else:
        try:
            wifi.create_ap(my_ssid, WIFI_AP_PASS)
            with UDPHandle("0.0.0.0", 54321) as udp:
                udp.era_info = {
                    "board":            board,
                    "model":            model,
                    "imei":             my_imei,
                    "firmware_version": fw_ver,
                    "ssid":             my_ssid,
                    "pass":             WIFI_AP_PASS,
                    "bssid":            wifi.mac_address(),
                    "ip":               "192.168.27.1"
                }
                udp.wifi_networks = wifi_networks
                udp.era_config = None
                if location == "SG":
                    udp.era_host = ERA_MQTT_HOST_SG
                else:
                    udp.era_host = ERA_MQTT_HOST_VN

                era_log("Waiting for ERa App connection...")
                while udp.era_config is None:
                    udp.handle_request()
                config = udp.era_config
        finally:
            wifi.remove_ap()

    if config is not None:
        wifi.set_hostname(my_ssid.replace(".", "-"))
        wifi.restart()
        time.sleep(3)
        wifi.scan()
        time.sleep(1)
        if "ssid" and "pass" in config:
            wifi.connect(config["ssid"], config["pass"])
        time.sleep(1)

    return config

if __name__ == "__main__":
    config = provision(sys.argv[1], sys.argv[2], sys.argv[3])
    print(json.dumps(config))