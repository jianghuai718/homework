// ex06 警车双通道反相渐变双闪
#define LED_A 2   // 第一路LED
#define LED_B 4   // 第二路LED

// 亮度变量 0~255
int brightness = 0;
// 增减步长，控制渐变快慢
int step = 2;

void setup() {
  Serial.begin(115200);
  pinMode(LED_A, OUTPUT);
  pinMode(LED_B, OUTPUT);
}

void loop() {
  // A灯亮度递增，B灯与A反相
  analogWrite(LED_A, brightness);
  analogWrite(LED_B, 255 - brightness);

  // 更新亮度值
  brightness += step;

  // 到达边界后反向，实现循环渐变
  if (brightness >= 255 || brightness <= 0) {
    step = -step;
  }

  // 控制渐变平滑度，数值越小过渡越顺滑
  delay(10);
}
