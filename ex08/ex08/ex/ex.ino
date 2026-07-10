#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// ==================== 热点配置 ====================
const char* AP_SSID     = "ESP32_Alarm";      // 热点名称
const char* AP_PASSWORD = "12345678";         // 热点密码（≥8位）

// ==================== 硬件引脚 ====================
#define ALARM_LED    2
#define TOUCH_CH     T0
const int TOUCH_THRESHOLD = 35;
const unsigned long TOUCH_DEBOUNCE = 60;

WebServer server(80);

// 全局状态变量
bool systemArmed = false;       // true=布防 false=撤防
bool alarmTriggered = false;    // true=锁定报警，无法自动解除
bool lastTouchState = false;
unsigned long touchDebounceTimer = 0;

// 分段生成网页
String getHtmlPage(){
  String page = "<!DOCTYPE html><html lang='zh-CN'>";
  page += "<head><meta charset='UTF-8'><title>ex08 安防报警器</title>";
  page += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";  // 手机适配
  page += "<style>body{text-align:center;margin-top:80px;font-size:22px;}";
  page += ".btn{font-size:24px;padding:15px 40px;margin:20px 10px;border:none;border-radius:8px;cursor:pointer;}";
  page += "#armBtn{background:#c0392b;color:white;}#disarmBtn{background:#27ae60;color:white;}";
  page += "#status{margin:30px 0;font-size:28px;font-weight:bold;}</style></head>";
  page += "<body><h2>物联网安防报警主机</h2>";
  page += "<div id='status'>系统状态：未布防</div><br>";
  page += "<button class='btn' id='armBtn'>布防 Arm</button>";
  page += "<button class='btn' id='disarmBtn'>撤防 Disarm</button>";
  page += "<script>const statusText = document.getElementById('status');";
  page += "document.getElementById('armBtn').onclick = ()=>{";
  page += "fetch('/arm');statusText.innerText = '系统状态：已布防，等待入侵检测';statusText.style.color='red';};";
  page += "document.getElementById('disarmBtn').onclick = ()=>{";
  page += "fetch('/disarm');statusText.innerText = '系统状态：未布防，警报已解除';statusText.style.color='#27ae60';};";
  page += "</script></body></html>";
  return page;
}

// 主页路由：返回安防控制页面
void handleRoot(){
  server.send(200, "text/html", getHtmlPage());
}

// 布防接口
void handleArm(){
  systemArmed = true;
  alarmTriggered = false;   // 布防时清除之前的报警状态
  digitalWrite(ALARM_LED, LOW);
  server.send(200, "text/plain", "armed");
}

// 撤防接口：清除报警、关闭LED、重置所有报警状态
void handleDisarm(){
  systemArmed = false;
  alarmTriggered = false;
  digitalWrite(ALARM_LED, LOW);
  server.send(200, "text/plain", "disarmed");
}

// 热点初始化（AP模式）
void initAP(){
  Serial.begin(115200);
  pinMode(ALARM_LED, OUTPUT);
  digitalWrite(ALARM_LED, LOW);

  Serial.print("开启热点: ");
  Serial.println(AP_SSID);
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  
  Serial.println("热点已开启！");
  Serial.print("安防主机IP：");
  Serial.println(WiFi.softAPIP());  // 默认 192.168.4.1
}

void setup(){
  initAP();
  // 注册网页接口
  server.on("/", handleRoot);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);
  server.begin();
  Serial.println("Web安防服务已启动");
  Serial.println("手机连接热点后访问 http://192.168.4.1");
}

void loop(){
  server.handleClient();
  unsigned long now = millis();

  // 报警锁定状态：高频闪烁LED，跳过触摸检测逻辑
  if(alarmTriggered){
    digitalWrite(ALARM_LED, HIGH);
    delay(80);
    digitalWrite(ALARM_LED, LOW);
    delay(80);
    return;
  }

  // 未报警时，触摸检测（防抖+上升沿）
  int touchVal = touchRead(TOUCH_CH);
  bool currTouch = (touchVal < TOUCH_THRESHOLD);

  if(now - touchDebounceTimer > TOUCH_DEBOUNCE){
    // 仅触摸上升沿触发判断
    if(currTouch == true && lastTouchState == false){
      // 只有布防状态才触发锁定报警
      if(systemArmed){
        alarmTriggered = true;
        Serial.println("入侵触发！报警锁定，请网页撤防解除");
      }
    }
    lastTouchState = currTouch;
    touchDebounceTimer = now;
  }
}