#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// ===== 热点配置 =====
const char* ap_ssid     = "ESP32_LED";      // 热点名称（手机搜这个）
const char* ap_password = "12345678";       // 热点密码（最少8位）
#define LED_PIN 2

WebServer server(80);
int ledBright = 0;

String getHtml(){
  String page = "<!DOCTYPE html><html lang='zh-CN'>";
  page += "<head><meta charset='UTF-8'><title>Web无极调光 ex07</title>";
  page += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";  // 让手机适配
  page += "<style>body { text-align:center; margin-top:80px; font-size:22px; }";
  page += "#slider { width:80%; height:30px; margin:30px 0; }";
  page += "#valText { font-size:26px; color:#0066cc; font-weight:bold; }</style></head>";
  page += "<body><h2>ESP32 网页滑动条无极调光</h2>";
  page += "<input type='range' id='slider' min='0' max='255' value='0'>";
  page += "<div>当前亮度值：<span id='valText'>0</span></div>";
  page += "<script>";
  page += "const slider = document.getElementById('slider');";
  page += "const valText = document.getElementById('valText');";
  page += "slider.addEventListener('input', function(){";
  page += "let val = this.value; valText.innerText = val; fetch('/set?bright='+val);";
  page += "});";
  page += "</script></body></html>";
  return page;
}

void handleRoot() {
  server.send(200, "text/html", getHtml());
}

void handleSetBright() {
  if(server.hasArg("bright")){
    ledBright = server.arg("bright").toInt();
    ledBright = constrain(ledBright, 0, 255);
    analogWrite(LED_PIN, ledBright);
  }
  server.send(200, "text/plain", "ok");
}

void setupAP(){
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  analogWrite(LED_PIN, 0);
  
  // ===== 开启热点（AP模式） =====
  Serial.print("开启热点: ");
  Serial.println(ap_ssid);
  WiFi.softAP(ap_ssid, ap_password);
  
  // 获取热点的IP地址（默认是 192.168.4.1）
  Serial.println("热点已开启！IP地址：");
  Serial.println(WiFi.softAPIP());
}

void setup() {
  setupAP();
  server.on("/", handleRoot);
  server.on("/set", handleSetBright);
  server.begin();
  Serial.println("Web服务器已启动！手机连接热点后，访问 http://192.168.4.1");
}

void loop() {
  server.handleClient();
}