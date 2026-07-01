// 引脚定义
#define LED_PIN     2       // LED输出引脚
#define TOUCH_PIN   T0      // 触摸通道0，对应GPIO4

// 状态变量
bool ledState = false;      // LED自锁状态
bool lastTouch = false;     // 上一轮触摸状态
unsigned long debounceTime = 0; // 防抖计时
const unsigned long debounceDelay = 50; // 防抖间隔50ms
const int touchThreshold = 600; // 触摸阈值，数值越小越灵敏

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, ledState); // 初始LED熄灭
}

void loop() {
  // 读取触摸原始值
  int touchVal = touchRead(TOUCH_PIN);
  bool currentTouch = (touchVal < touchThreshold); // 低于阈值判定为触摸

  unsigned long now = millis();
  // 防抖判断：距离上次状态变化超过防抖延时才更新状态
  if (now - debounceTime > debounceDelay) {
    // 边缘检测：上一次未触摸，当前触摸 → 单次按下触发
    if (currentTouch == true && lastTouch == false) {
      ledState = !ledState; // 翻转LED状态
      digitalWrite(LED_PIN, ledState);
      Serial.print("触摸触发，LED状态切换：");
      Serial.println(ledState ? "亮" : "灭");
      debounceTime = now; // 更新防抖计时
    }
    // 更新历史触摸状态
    lastTouch = currentTouch;
  }

  // 打印调试信息（可选）
  Serial.print("触摸值：");
  Serial.print(touchVal);
  Serial.print(" | 当前触摸：");
  Serial.println(currentTouch ? "是" : "否");
  delay(20);
}