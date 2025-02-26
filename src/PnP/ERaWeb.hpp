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
"x.type = 'password'; }"
"x = document.getElementById('backup_pass');"
"if (x.type == 'password') {"
"x.type = 'text';"
"} else {"
"x.type = 'password'; }"
"x = document.getElementById('password');"
"if (x.type == 'password') {"
"x.type = 'text';"
"} else {"
"x.type = 'password'; }}"
"function clicked(e) {"
"var id = document.getElementById('ssid');"
"var pass = document.getElementById('pass');"
"var auth = document.getElementById('token');"
"let selected = undefined;"
"if (id?.tagName === 'SELECT') {"
"selected = id?.options[id.selectedIndex]; }"
"if (id.value === '') {"
"alert('%NONE_WIFI_MSG%');"
"e.preventDefault();"
"} else if (selected?.text.includes('*') && pass.value === '') {"
"alert('%NONE_PASS_MSG%');"
"e.preventDefault();"
"} else if (auth.value === '') {"
"alert('%NONE_AUTH_MSG%');"
"e.preventDefault();"
"} else {"
"if (!confirm('%WIFI_MSG% \"' + id.value + '\"?')) {"
"e.preventDefault(); }}}"
"function clickedSave(e) {"
"if (!confirm('%SAVE_MSG%')) {"
"e.preventDefault(); }}"
"function clickedOTA(e) {"
"var url = document.getElementById('url');"
"var md5 = document.getElementById('md5');"
"var type = document.getElementById('type');"
"if (url.value === '') {"
"alert('%NONE_OTA_URL_MSG%');"
"e.preventDefault();"
"} else if (md5.value === '') {"
"alert('%NONE_OTA_MD5_MSG%');"
"e.preventDefault();"
"} else if (type.value === '') {"
"alert('%NONE_OTA_TYPE_MSG%');"
"e.preventDefault();"
"} else {"
"if (!confirm('%UPDATE_MSG% \"' + url.value + '\"?')) {"
"e.preventDefault(); }}}"
"function resetNetwork(e) {"
"if (!confirm('%RESET_NETWORK_MSG%?')) {"
"e.preventDefault(); }}"
"var time = 15;"
"var page = 'https://e-ra.io/';"
"function countDown() {"
"time--;"
"if (time <= 0){"
"time = 0;"
"window.location = page; }"
"getCon('container').innerHTML = time; }"
"function getCon(id) {"
"if (document.getElementById) return document.getElementById(id);"
"if (document.all) return document.all.id;"
"if (document.layers) return document.layers.id;"
"if (window.opera) return window.opera.id; }"
"function init() {"
"if (getCon('container')) {"
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
"<tr><td>MQTT</td>"
"<td><span id='mqtt'>%LOADING%</span></td></tr>"
"<tr><td>Memory</td>"
"<td><span id='memory'>%LOADING%</span></td></tr>"
"<tr><td>Uptime</td>"
"<td><span id='uptime'>%LOADING%</span></td></tr>"
"<tr><td>Token</td>"
"<td><span id='token'>%LOADING%</span></td></tr>"
"<tr><td>Version</td>"
"<td>" ERA_VERSION "</td></tr>"
"<tr><td>Firmware</td>"
"<td>" ERA_FIRMWARE_VERSION "</td></tr>"
"</table>"
"<input type=submit formaction='/network' class=btn value=Network>"
"<input type=submit formaction='/network/reset' onclick=resetNetwork(event) class=btn value='%RESET%'>"
"<input type=submit formaction='/system' class=btn value=System>"
"<input type=submit formaction='/ota' class=btn value=Update>"
"<input type=submit formaction='/ota/url' class=btn value='Update URL'></form>"
"<script>"
"function interval() {"
"getNetStatus();"
"getIP();"
"getSSID();"
"getRSSI();"
"getMac();"
"getMQTT();"
"getMemory();"
"getUptime();"
"getToken(); }"
"interval();"
"setInterval(interval, 10000);"
"function getNetStatus() {"
"var xhttp = new XMLHttpRequest();"
"xhttp.onreadystatechange = function() {"
"if (this.readyState == 4 && this.status == 200) {"
"document.getElementById('net').innerHTML ="
"this.responseText; }};"
"xhttp.open('POST', '/network/status', true);"
"xhttp.send(); }"
"function getIP() {"
"var xhttp = new XMLHttpRequest();"
"xhttp.onreadystatechange = function() {"
"if (this.readyState == 4 && this.status == 200) {"
"document.getElementById('ip').innerHTML ="
"this.responseText; }};"
"xhttp.open('POST', '/network/ip', true);"
"xhttp.send(); }"
"function getSSID() {"
"var xhttp = new XMLHttpRequest();"
"xhttp.onreadystatechange = function() {"
"if (this.readyState == 4 && this.status == 200) {"
"document.getElementById('ssid').innerHTML ="
"this.responseText; }};"
"xhttp.open('POST', '/network/ssid', true);"
"xhttp.send(); }"
"function getRSSI() {"
"var xhttp = new XMLHttpRequest();"
"xhttp.onreadystatechange = function() {"
"if (this.readyState == 4 && this.status == 200) {"
"document.getElementById('rssi').innerHTML ="
"this.responseText; }};"
"xhttp.open('POST', '/network/rssi', true);"
"xhttp.send(); }"
"function getMac() {"
"var xhttp = new XMLHttpRequest();"
"xhttp.onreadystatechange = function() {"
"if (this.readyState == 4 && this.status == 200) {"
"document.getElementById('mac').innerHTML ="
"this.responseText; }};"
"xhttp.open('POST', '/network/mac', true);"
"xhttp.send(); }"
"function getMQTT() {"
"var xhttp = new XMLHttpRequest();"
"xhttp.onreadystatechange = function() {"
"if (this.readyState == 4 && this.status == 200) {"
"document.getElementById('mqtt').innerHTML ="
"this.responseText; }};"
"xhttp.open('POST', '/mqtt/connected', true);"
"xhttp.send(); }"
"function getMemory() {"
"var xhttp = new XMLHttpRequest();"
"xhttp.onreadystatechange = function() {"
"if (this.readyState == 4 && this.status == 200) {"
"document.getElementById('memory').innerHTML ="
"this.responseText; }};"
"xhttp.open('POST', '/system/memory', true);"
"xhttp.send(); }"
"function getUptime() {"
"var xhttp = new XMLHttpRequest();"
"xhttp.onreadystatechange = function() {"
"if (this.readyState == 4 && this.status == 200) {"
"document.getElementById('uptime').innerHTML ="
"this.responseText; }};"
"xhttp.open('POST', '/system/uptime', true);"
"xhttp.send(); }"
"function getToken() {"
"var xhttp = new XMLHttpRequest();"
"xhttp.onreadystatechange = function() {"
"if (this.readyState == 4 && this.status == 200) {"
"document.getElementById('token').innerHTML ="
"this.responseText; }};"
"xhttp.open('POST', '/system/token', true);"
"xhttp.send(); }"
"</script>";

static const char webOTA[] ERA_PROGMEM = 
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<meta charset='utf-8'>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
"<h1>%UPDATE_CAP%</h1>"
"<input type='file' name='update' id='file' onchange='sub(this)' style=display:none>"
"<label id='file-input' for='file'>%CHOOSE_FILE%</label>"
"<input type='submit' class=btn value='%UPDATE%'>"
"<br><br>"
"<div id='prg'></div>"
"<br><div id='prgbar'><div id='bar'></div></div><br></form>"
"<script>"
"function systemRestart() {"
"var xhttp = new XMLHttpRequest();"
"xhttp.open('POST', '/system/restart', true);"
"xhttp.send(); }"
"function sub(obj){"
"var fileName = obj.value.split('\\\\');"
"document.getElementById('file-input').innerHTML = fileName[fileName.length-1];"
"};"
"$('form').submit(function(e){"
"var fileName = document.getElementById('file-input').innerHTML;"
"if (fileName === '%CHOOSE_FILE%'){"
"alert('%NONE_FILE_MSG%');"
"e.preventDefault();"
"}else{"
"if (confirm('%UPDATE_MSG% \"' + fileName + '\"?')) {"
"e.preventDefault();"
"var form = $('#upload_form')[0];"
"var data = new FormData(form);"
"$.ajax({"
"url: '/ota/update',"
"method: 'POST',"
"type: 'POST',"
"data: data,"
"contentType: false,"
"processData: false,"
"xhr: function() {"
"var xhr = new window.XMLHttpRequest();"
"xhr.upload.addEventListener('progress', function(evt) {"
"if (evt.lengthComputable) {"
"var per = evt.loaded / evt.total;"
"$('#prg').html('%PROGRESS%: ' + Math.round(per*100) + '%');"
"$('#bar').css('width', Math.round(per*100) + '%');"
"}"
"}, false);"
"return xhr;"
"},"
"success:function(data) {"
"alert(data);"
"systemRestart();"
"},"
"error:function (data) {"
"}"
"}); }"
"else{e.preventDefault(); }}"
"});"
"</script>";

enum LanguageWebT {
    LANGUAGE_EN = 0,
    LANGUAGE_VN = 1
};

#define LANGUAGE_FEATURES(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Features" : "Tính năng"
#define LANGUAGE_STATUS(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Status" : "Trạng thái"
#define LANGUAGE_NETWORK(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Network" : "Kết nối mạng"
#define LANGUAGE_NETWORK_CAP(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "NETWORK" : "KẾT NỐI MẠNG"
#define LANGUAGE_BACK(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Back" : "Quay lại"
#define LANGUAGE_CONNECT(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Connect" : "Kết nối"
#define LANGUAGE_CONNECTED(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Connected" : "Đã kết nối"
#define LANGUAGE_DISCONNECT(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Disconnect" : "Mất kết nối"
#define LANGUAGE_DISCONNECTED(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Disconnected" : "Đã mất kết nối"
#define LANGUAGE_SAVE(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Save" : "Lưu"
#define LANGUAGE_SSID(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "SSID" : "Tên WiFi"
#define LANGUAGE_SELECT_SSID(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Select your ssid" : "Chọn WiFi của bạn"
#define LANGUAGE_PASSWORD(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Password" : "Mật khẩu"
#define LANGUAGE_BACKUP_SSID(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "SSID 2" : "Tên WiFi 2"
#define LANGUAGE_SELECT_BACKUP_SSID(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Select your ssid 2" : "Chọn WiFi 2 của bạn"
#define LANGUAGE_BACKUP_PASS(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Password 2" : "Mật khẩu 2"
#define LANGUAGE_SHOW_PASSWORD(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Show password" : "Hiện mật khẩu"
#define LANGUAGE_SCAN_NETWORK(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Scan network" : "Quét mạng"
#define LANGUAGE_SAVE_MSG(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Do you want force save?" : "Bạn có muốn lưu?"
#define LANGUAGE_WIFI_MSG(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Do you want connect to" : "Bạn có muốn kết nối"
#define LANGUAGE_NONE_WIFI_MSG(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Please choose WiFi!" : "Vui lòng chọn WiFi!"
#define LANGUAGE_NONE_PASS_MSG(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Please enter the password!" : "Vui lòng nhập mật khẩu!"
#define LANGUAGE_NONE_AUTH_MSG(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Please provide the token!" : "Vui lòng cung cấp mã token!"
#define LANGUAGE_ONLINE(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Online" : "Trực tuyến"
#define LANGUAGE_OFFLINE(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Offline" : "Ngoại tuyến"
#define LANGUAGE_HOST(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Host" : "Máy chủ"
#define LANGUAGE_PORT(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Port" : "Cổng kết nối"
#define LANGUAGE_TOKEN(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Token" : "Token"
#define LANGUAGE_USERNAME(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Username" : "Tài khoản"
#define LANGUAGE_PASSWORD(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Password" : "Mật khẩu"
#define LANGUAGE_RESET_NETWORK_MSG(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Do you want reset network" : "Bạn có muốn khôi phục kết nối mạng"
#define LANGUAGE_RESET(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Reset" : "Khôi phục"
#define LANGUAGE_LOADING(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Loading..." : "Đang lấy dữ liệu..."
#define LANGUAGE_FAIL(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Fail" : "Thất bại"
#define LANGUAGE_UPDATE(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Update" : "Cập nhật"
#define LANGUAGE_UPDATE_CAP(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "UPDATE" : "CẬP NHẬT"
#define LANGUAGE_UPDATE_MSG(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Do you want update firmware" : "Bạn có muốn cập nhật firmware"
#define LANGUAGE_UPDATE_SUCCESS(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Update success!" : "Cập nhật thành công!"
#define LANGUAGE_UPDATE_FAIL(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Update fail!" : "Cập nhật thất bại!"
#define LANGUAGE_CHOOSE_FILE(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Choose file..." : "Chọn file..."
#define LANGUAGE_NONE_FILE_MSG(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Please choose firmware!" : "Vui lòng chọn phần mềm!"
#define LANGUAGE_PROGRESS(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Progress" : "Tiến trình"
#define LANGUAGE_URL(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "URL" : "Đường dẫn"
#define LANGUAGE_SELECT_OTA_TYPE(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Select your type OTA" : "Chọn kiểu cập nhật của bạn"
#define LANGUAGE_NONE_OTA_URL_MSG(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Please provide the URL!" : "Vui lòng cung cấp URL!"
#define LANGUAGE_NONE_OTA_TYPE_MSG(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Please choose the OTA type!" : "Vui lòng chọn kiểu cập nhật!"
#define LANGUAGE_NONE_OTA_MD5_MSG(lang) (lang == LanguageWebT::LANGUAGE_EN) ? "Please provide the MD5 hash!" : "Vui lòng cung cấp mã MD5!"

inline
static void WebProcessor(String& str, uint8_t language = LANGUAGE_EN) {
    str.replace("%FEATURES%", LANGUAGE_FEATURES(language));
    str.replace("%STATUS%", LANGUAGE_STATUS(language));
    str.replace("%NETWORK%", LANGUAGE_NETWORK(language));
    str.replace("%NETWORK_CAP%", LANGUAGE_NETWORK_CAP(language));
    str.replace("%BACK%", LANGUAGE_BACK(language));
    str.replace("%CONNECT%", LANGUAGE_CONNECT(language));
    str.replace("%CONNECTED%", LANGUAGE_CONNECTED(language));
    str.replace("%DISCONNECTED%", LANGUAGE_DISCONNECTED(language));
    str.replace("%SAVE%", LANGUAGE_SAVE(language));
    str.replace("%SSID%", LANGUAGE_SSID(language));
    str.replace("%PASSWORD%", LANGUAGE_PASSWORD(language));
    str.replace("%SELECT_SSID%", LANGUAGE_SELECT_SSID(language));
    str.replace("%BACKUP_SSID%", LANGUAGE_BACKUP_SSID(language));
    str.replace("%BACKUP_PASS%", LANGUAGE_BACKUP_PASS(language));
    str.replace("%SELECT_BACKUP_SSID%", LANGUAGE_SELECT_BACKUP_SSID(language));
    str.replace("%SHOW_PASSWORD%", LANGUAGE_SHOW_PASSWORD(language));
    str.replace("%SCAN_NETWORK%", LANGUAGE_SCAN_NETWORK(language));
    str.replace("%SAVE_MSG%", LANGUAGE_SAVE_MSG(language));
    str.replace("%WIFI_MSG%", LANGUAGE_WIFI_MSG(language));
    str.replace("%NONE_WIFI_MSG%", LANGUAGE_NONE_WIFI_MSG(language));
    str.replace("%NONE_PASS_MSG%", LANGUAGE_NONE_PASS_MSG(language));
    str.replace("%NONE_AUTH_MSG%", LANGUAGE_NONE_AUTH_MSG(language));
    str.replace("%RESET_NETWORK_MSG%", LANGUAGE_RESET_NETWORK_MSG(language));
    str.replace("%HOST%", LANGUAGE_HOST(language));
    str.replace("%PORT%", LANGUAGE_PORT(language));
    str.replace("%TOKEN%", LANGUAGE_TOKEN(language));
    str.replace("%USERNAME%", LANGUAGE_USERNAME(language));
    str.replace("%PASSWORD%", LANGUAGE_PASSWORD(language));
    str.replace("%RESET%", LANGUAGE_RESET(language));
    str.replace("%LOADING%", LANGUAGE_LOADING(language));
    str.replace("%UPDATE%", LANGUAGE_UPDATE(language));
    str.replace("%UPDATE_CAP%", LANGUAGE_UPDATE_CAP(language));
    str.replace("%UPDATE_MSG%", LANGUAGE_UPDATE_MSG(language));
    str.replace("%CHOOSE_FILE%", LANGUAGE_CHOOSE_FILE(language));
    str.replace("%NONE_FILE_MSG%", LANGUAGE_NONE_FILE_MSG(language));
    str.replace("%PROGRESS%", LANGUAGE_PROGRESS(language));
    str.replace("%SELECT_OTA_TYPE%", LANGUAGE_SELECT_OTA_TYPE(language));
    str.replace("%NONE_OTA_URL_MSG%", LANGUAGE_NONE_OTA_URL_MSG(language));
    str.replace("%NONE_OTA_TYPE_MSG%", LANGUAGE_NONE_OTA_TYPE_MSG(language));
    str.replace("%NONE_OTA_MD5_MSG%", LANGUAGE_NONE_OTA_MD5_MSG(language));
}

#endif /* INC_ERA_WEB_HPP_ */
