#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char* ssid     = "Mark7";
const char* password = "zxy623718";
#define LED_PIN 2
WebServer server(80);
int ledBright = 0;

String getHtml(){
  String page = "<!DOCTYPE html><html lang='zh-CN'>";
  page += "<head><meta charset='UTF-8'><title>Web无极调光 ex07</title>";
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

void setupWiFi(){
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  analogWrite(LED_PIN, 0);
  Serial.print("连接WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi连接成功！IP：");
  Serial.println(WiFi.localIP());
}

void setup() {
  setupWiFi();
  server.on("/", handleRoot);
  server.on("/set", handleSetBright);
  server.begin();
}

void loop() {
  server.handleClient();
}