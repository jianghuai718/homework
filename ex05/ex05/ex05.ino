// 引脚定义
#define LED_PIN     2
#define TOUCH_PIN   T0

// 触摸防抖变量
bool lastTouch = false;
unsigned long touchDebounceTimer = 0;
const unsigned long touchDebounceDelay = 60;
const int touchThreshold = 35;

// 呼吸灯档位与参数
int speedLevel = 1;    // 档位 1/2/3
int ledBright = 0;     // PWM亮度 0~255
int fadeDir = 1;       // 1渐亮，-1渐暗

// 三档呼吸步长，档位越高步长越大，呼吸越快
int stepTable[4] = {0, 1, 3, 6};  

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  analogWrite(LED_PIN, ledBright);
  Serial.println("ex05 多档位触摸调速呼吸灯启动");
  Serial.println("档位1：慢速 | 档位2：中速 | 档位3：快速");
}

void loop() {
  // ========== 触摸检测、档位切换（复用ex04边缘防抖逻辑） ==========
  int touchRaw = touchRead(TOUCH_PIN);
  bool currentTouch = (touchRaw < touchThreshold);
  unsigned long now = millis();

  if (now - touchDebounceTimer > touchDebounceDelay) {
    // 上升沿：未触摸 → 触摸瞬间切换档位
    if (currentTouch == true && lastTouch == false) {
      speedLevel++;
      if (speedLevel > 3) speedLevel = 1; // 1→2→3→1循环
      Serial.print("切换档位：");
      Serial.println(speedLevel);
      touchDebounceTimer = now;
    }
    lastTouch = currentTouch;
  }

  // ========== PWM呼吸灯（实验3基础）根据档位改变速度 ==========
  int step = stepTable[speedLevel]; // 获取当前档位步长
  ledBright += fadeDir * step;

  // 亮度边界翻转方向
  if (ledBright >= 255) {
    ledBright = 255;
    fadeDir = -1;
  } else if (ledBright <= 0) {
    ledBright = 0;
    fadeDir = 1;
  }

  analogWrite(LED_PIN, ledBright);
  delay(8); // 基础延时，配合步长控制整体呼吸速率
}