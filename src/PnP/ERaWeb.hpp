#ifndef INC_ERA_WEB_HPP_
#define INC_ERA_WEB_HPP_

static const char webStyle[] ERA_PROGMEM =
"<style>#file-input,#file-server-cert,#file-client-cert,#file-client-key,input,select{width:100%;height:44px;border-radius:4px;margin:10px auto;font-size:15px}"
"table{width:100%;height:44px;border-radius:4px;margin:10px auto;font-size:15px;table-layout:fixed;border-collapse:collapse}"
"td:hover{background-color:#3498db;color:white}th{background-color:gray;color:white;}"
"tr:nth-child(even){background-color:#bae3ff}tr:nth-child(odd){background-color:#cceaff}"
"input,select{background:#f1f1f1;border:0;padding:0 15px}body{background:#3498db;font-family:sans-serif;font-size:14px;color:#777}"
"#file-input,#file-server-cert,#file-client-cert,#file-client-key{padding:0;border:1px solid #ddd;line-height:44px;text-align:left;display:block;cursor:pointer}"
"#bar,#prgbar{background-color:#f1f1f1;border-radius:10px}#bar{background-color:#3498db;width:0%;height:10px}"
"form{background:#fff;max-width:500px;margin:75px auto;padding:30px;border-radius:5px;text-align:center}"
".btn{background:#3498db;color:#fff;cursor:pointer}.checkbox{height:20px;width:20px;vertical-align:middle}"
".crop{width:350px;height:350px;overflow:hidden;border:1px solid black;margin:auto;}"
".crop img{width:450px;height:450px;margin:-50px -50px -50px -50px;}"
"textarea{-webkit-box-sizing:border-box;-moz-box-sizing:border-box;box-sizing:border-box;width:100%;max-width:100%;}</style>";

static const char webScript[] ERA_PROGMEM =
"<script type='text/javascript'>"
"function togglePass() {"
"var x = document.getElementById('pass');"
"if (x.type == 'password') {"
"x.type = 'text';"
"} else {"
"x.type = 'password';}"
"x = document.getElementById('backup_pass');"
"if (x.type == 'password') {"
"x.type = 'text';"
"} else {"
"x.type = 'password';}}"
"function clicked(e) {"
"var id = document.getElementById('ssid');"
"if(id.value === '') {"
"alert('%NONE_WIFI_MSG%');"
"e.preventDefault();"
"}else{"
"if(!confirm('%WIFI_MSG% \"' + id.value + '\"?')) {"
"e.preventDefault();}}}"
"function resetWifi(e) {"
"if(!confirm('%RESET_WIFI_MSG%?')) {"
"e.preventDefault();}}"
"var time = 15;"
"var page = 'https://e-ra.io/';"
"function countDown() {"
"time--;"
"if(time <= 0){"
"time = 0;"
"window.location = page;}"
"getCon('container').innerHTML = time;}"
"function getCon(id) {"
"if(document.getElementById) return document.getElementById(id);"
"if(document.all) return document.all.id;"
"if(document.layers) return document.layers.id;"
"if(window.opera) return window.opera.id;}"
"function init() {"
"if(getCon('container')) {"
"setInterval(countDown, 1000);"
"getCon('container').innerHTML = time;"
"}else{"
"setTimeout(init, 50)}};"
"document.onload = init();"
"</script>";

static const char webIndex[] ERA_PROGMEM = 
"<meta charset='utf-8'>"
"<form method='post'>"
"<h1>%NETWORK_CAP%</h1>"
"<table border=1 width='100%'>"
"<tr><th width='40%' height='30px'>%FEATURES%</th>"
"<th width='60%'>%STATUS%</th></tr>"
"<tr><td>%NETWORK%</td>"
"<td><span id='net'>%LOADING%</span></td></tr>"
"<tr><td>IP</td>"
"<td><span id='ip'>%LOADING%</span></td></tr>"
"<tr><td>SSID</td>"
"<td><span id='ssid'>%LOADING%</span></td></tr>"
"<tr><td>RSSI</td>"
"<td><span id='rssi'>%LOADING%</span></td></tr>"
"<tr><td>MAC</td>"
"<td><span id='mac'>%LOADING%</span></td></tr>"
"</table>"
"<input type=submit formaction='/wifi' class=btn value=Wifi>"
"<input type=submit formaction='/resetWifi' onclick=resetWifi(event) class=btn value='%RESET%'></form>"
"<script>"
"function interval() {"
"getNetStatus();"
"getIP();"
"getSSID();"
"getRSSI();"
"getMac();}"
"interval();"
"setInterval(interval, 10000);"
"function getNetStatus() {"
"var xhttp = new XMLHttpRequest();"
"xhttp.onreadystatechange = function() {"
"if (this.readyState == 4 && this.status == 200) {"
"document.getElementById('net').innerHTML ="
"this.responseText;}};"
"xhttp.open('POST', '/network/status', true);"
"xhttp.send();}"
"function getIP() {"
"var xhttp = new XMLHttpRequest();"
"xhttp.onreadystatechange = function() {"
"if (this.readyState == 4 && this.status == 200) {"
"document.getElementById('ip').innerHTML ="
"this.responseText;}};"
"xhttp.open('POST', '/network/ip', true);"
"xhttp.send();}"
"function getSSID() {"
"var xhttp = new XMLHttpRequest();"
"xhttp.onreadystatechange = function() {"
"if (this.readyState == 4 && this.status == 200) {"
"document.getElementById('ssid').innerHTML ="
"this.responseText;}};"
"xhttp.open('POST', '/network/ssid', true);"
"xhttp.send();}"
"function getRSSI() {"
"var xhttp = new XMLHttpRequest();"
"xhttp.onreadystatechange = function() {"
"if (this.readyState == 4 && this.status == 200) {"
"document.getElementById('rssi').innerHTML ="
"this.responseText;}};"
"xhttp.open('POST', '/network/rssi', true);"
"xhttp.send();}"
"function getMac() {"
"var xhttp = new XMLHttpRequest();"
"xhttp.onreadystatechange = function() {"
"if (this.readyState == 4 && this.status == 200) {"
"document.getElementById('mac').innerHTML ="
"this.responseText;}};"
"xhttp.open('POST', '/network/mac', true);"
"xhttp.send();}"
"</script>";

enum {
    LANGUAGE_EN = 0,
    LANGUAGE_VN = 1
};

#define LANGUAGE_FEATURES(lang) (lang == LANGUAGE_EN) ? "Features" : "Tính năng"
#define LANGUAGE_STATUS(lang) (lang == LANGUAGE_EN) ? "Status" : "Trạng thái"
#define LANGUAGE_NETWORK(lang) (lang == LANGUAGE_EN) ? "Network" : "Kết nối mạng"
#define LANGUAGE_NETWORK_CAP(lang) (lang == LANGUAGE_EN) ? "NETWORK" : "KẾT NỐI MẠNG"
#define LANGUAGE_BACK(lang) (lang == LANGUAGE_EN) ? "Back" : "Quay lại"
#define LANGUAGE_CONNECT(lang) (lang == LANGUAGE_EN) ? "Connect" : "Kết nối"
#define LANGUAGE_CONNECTED(lang) (lang == LANGUAGE_EN) ? "Connected" : "Đã kết nối"
#define LANGUAGE_DISCONNECT(lang) (lang == LANGUAGE_EN) ? "Disconnect" : "Mất kết nối"
#define LANGUAGE_DISCONNECTED(lang) (lang == LANGUAGE_EN) ? "Disconnected" : "Đã mất kết nối"
#define LANGUAGE_SSID(lang) (lang == LANGUAGE_EN) ? "SSID" : "Tên wifi"
#define LANGUAGE_SELECT_SSID(lang) (lang == LANGUAGE_EN) ? "Select your ssid" : "Chọn wifi của bạn"
#define LANGUAGE_PASSWORD(lang) (lang == LANGUAGE_EN) ? "Password" : "Mật khẩu"
#define LANGUAGE_BACKUP_SSID(lang) (lang == LANGUAGE_EN) ? "SSID 2" : "Tên wifi 2"
#define LANGUAGE_SELECT_BACKUP_SSID(lang) (lang == LANGUAGE_EN) ? "Select your ssid 2" : "Chọn wifi 2 của bạn"
#define LANGUAGE_BACKUP_PASS(lang) (lang == LANGUAGE_EN) ? "Password 2" : "Mật khẩu 2"
#define LANGUAGE_SHOW_PASSWORD(lang) (lang == LANGUAGE_EN) ? "Show password" : "Hiện mật khẩu"
#define LANGUAGE_SCAN_WIFI(lang) (lang == LANGUAGE_EN) ? "Scan Wifi" : "Quét Wifi"
#define LANGUAGE_WIFI_MSG(lang) (lang == LANGUAGE_EN) ? "Do you want connect to" : "Bạn có muốn kết nối"
#define LANGUAGE_NONE_WIFI_MSG(lang) (lang == LANGUAGE_EN) ? "Please choose wifi!" : "Vui lòng chọn wifi!"
#define LANGUAGE_ONLINE(lang) (lang == LANGUAGE_EN) ? "Online" : "Trực tuyến"
#define LANGUAGE_OFFLINE(lang) (lang == LANGUAGE_EN) ? "Offline" : "Ngoại tuyến"
#define LANGUAGE_RESET_WIFI_MSG(lang) (lang == LANGUAGE_EN) ? "Do you want reset wifi" : "Bạn có muốn khôi phục wifi"
#define LANGUAGE_RESET(lang) (lang == LANGUAGE_EN) ? "Reset" : "Khôi phục"
#define LANGUAGE_LOADING(lang) (lang == LANGUAGE_EN) ? "Loading..." : "Đang lấy dữ liệu..."
#define LANGUAGE_FAIL(lang) (lang == LANGUAGE_EN) ? "Fail" : "Thất bại"

inline
static void webProcessor(String& str, uint8_t language = LANGUAGE_EN) {
    str.replace("%FEATURES%", LANGUAGE_FEATURES(language));
    str.replace("%STATUS%", LANGUAGE_STATUS(language));
    str.replace("%NETWORK%", LANGUAGE_NETWORK(language));
    str.replace("%NETWORK_CAP%", LANGUAGE_NETWORK_CAP(language));
    str.replace("%BACK%", LANGUAGE_BACK(language));
    str.replace("%CONNECT%", LANGUAGE_CONNECT(language));
    str.replace("%CONNECTED%", LANGUAGE_CONNECTED(language));
    str.replace("%DISCONNECTED%", LANGUAGE_DISCONNECTED(language));
    str.replace("%SSID%", LANGUAGE_SSID(language));
    str.replace("%PASSWORD%", LANGUAGE_PASSWORD(language));
    str.replace("%SELECT_SSID%", LANGUAGE_SELECT_SSID(language));
    str.replace("%BACKUP_SSID%", LANGUAGE_BACKUP_SSID(language));
    str.replace("%BACKUP_PASS%", LANGUAGE_BACKUP_PASS(language));
    str.replace("%SELECT_BACKUP_SSID%", LANGUAGE_SELECT_BACKUP_SSID(language));
    str.replace("%SHOW_PASSWORD%", LANGUAGE_SHOW_PASSWORD(language));
    str.replace("%SCAN_WIFI%", LANGUAGE_SCAN_WIFI(language));
    str.replace("%WIFI_MSG%", LANGUAGE_WIFI_MSG(language));
    str.replace("%NONE_WIFI_MSG%", LANGUAGE_NONE_WIFI_MSG(language));
    str.replace("%RESET_WIFI_MSG%", LANGUAGE_RESET_WIFI_MSG(language));
    str.replace("%RESET%", LANGUAGE_RESET(language));
    str.replace("%LOADING%", LANGUAGE_LOADING(language));
}

#endif /* INC_ERA_WEB_HPP_ */
