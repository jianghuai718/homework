#include <WiFi.h>
#include <WebServer.h>

// ==================== 热点配置 ====================
const char* AP_SSID     = "ESP32_Touch";      // 热点名称
const char* AP_PASSWORD = "12345678";         // 热点密码（≥8位）

#define TOUCH_CH     T0
const int TOUCH_THRESHOLD = 600;
const unsigned long TOUCH_DEBOUNCE = 60;

WebServer server(80);
int touchRawValue = 0;

String getDashboardHtml(){
  String page = "<!DOCTYPE html><html lang='zh-CN'>";
  page += "<head><meta charset='UTF-8'><title>ex09 实时传感器仪表盘</title>";
  page += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";  // 手机适配
  page += "<style>body {background:#121212;color:#fff;text-align:center;margin-top:100px;font-family:Arial;}h2 {font-size:32px;color:#00eeff;}#dataBox {font-size:80px;margin:40px auto;padding:40px;width:60%;background:#1e1e1e;border-radius:16px;border:3px solid #00eeff;}.tip {font-size:20px;color:#aaa;margin-top:30px;}</style></head>";
  page += "<body><h2>ESP32 触摸传感器实时仪表盘</h2><div id='dataBox'>等待数据...</div><div class='tip'>靠近触摸引脚 → 数值变小 | 松开手指 → 数值回升</div>";
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

// 热点初始化（AP模式）
void initAP(){
  Serial.begin(115200);
  Serial.print("开启热点：");
  Serial.println(AP_SSID);
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  Serial.println("热点已开启！");
  Serial.print("仪表盘访问IP：");
  Serial.println(WiFi.softAPIP());  // 默认 192.168.4.1
}

void setup(){
  initAP();
  server.on("/", handleDashboard);
  server.on("/getData", handleGetData);
  server.begin();
  Serial.println("Web仪表盘服务启动完成");
  Serial.println("手机连接热点后访问 http://192.168.4.1");
}

void loop(){
  server.handleClient();
  touchRawValue = touchRead(TOUCH_CH);
  delay(50);
}