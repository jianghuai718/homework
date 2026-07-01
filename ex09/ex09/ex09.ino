#include <WiFi.h>
#include <WebServer.h>

const char* WIFI_SSID     = "Mark7";
const char* WIFI_PASSWORD = "zxy623718";

#define TOUCH_CH     T0
const int TOUCH_THRESHOLD = 600;
const unsigned long TOUCH_DEBOUNCE = 60;

WebServer server(80);
int touchRawValue = 0;

String getDashboardHtml(){
  String page = "<!DOCTYPE html><html lang='zh-CN'>";
  page += "<head><meta charset='UTF-8'><title>ex09 实时传感器仪表盘</title>";
  page += "<style>body {background:#121212;color:#fff;text-align:center;margin-top:100px;font-family:Arial;}h2 {font-size:32px;color:#00eeff;}#dataBox {font-size:80px;margin:40px auto;padding:40px;width:60%;background:#1e1e1e;border-radius:16px;border:3px solid #00eeff;}.tip {font-size:20px;color:#aaa;margin-top:30px;}</style></head>";
  page += "<body><h2>ESP32 触摸传感器实时仪表盘</h2><div id='dataBox'>等待数据...</div><div class='tip'>靠近触摸引脚 → 数值变小 | 松开手指 → 数值回升</div>";
  // JS整行拼接，不换行，杜绝Arduino误解析
  page += "<script>function fetchSensorData(){fetch('/getData').then(res=>res.text()).then(val=>{document.getElementById('dataBox').innerText=val;}).catch(err=>console.log('连接断开'));}setInterval(fetchSensorData,100);fetchSensorData();</script>";
  page += "</body></html>";
  return page;
}

void handleDashboard(){
  server.send(200, "text/html", getDashboardHtml());
}

void handleGetData(){
  touchRawValue = touchRead(TOUCH_CH);
  server.send(200, "text/plain", String(touchRawValue));
}

void initWiFi(){
  Serial.begin(115200);
  Serial.print("正在连接WiFi：");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi连接成功！");
  Serial.print("仪表盘访问IP：");
  Serial.println(WiFi.localIP());
}

void setup(){
  initWiFi();
  server.on("/", handleDashboard);
  server.on("/getData", handleGetData);
  server.begin();
  Serial.println("Web仪表盘服务启动完成");
}

void loop(){
  server.handleClient();
  touchRawValue = touchRead(TOUCH_CH);
  delay(50);
}