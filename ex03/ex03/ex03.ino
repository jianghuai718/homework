const int LED_PIN = 2;

// SOS时序 (单位: 毫秒)
const int DOT = 200;        // 短闪
const int DASH = 600;       // 长闪 (3倍点)
const int SYMBOL_GAP = 200; // 符号间间隔
const int LETTER_GAP = 600; // 字母间间隔
const int WORD_GAP = 2000;  // 单词间停顿 (较长熄灭)

enum SOSState {
  S_DOT_1, S_GAP_1,
  S_DOT_2, S_GAP_2,
  S_DOT_3, S_GAP_3,
  S_DASH_1, S_GAP_4,
  S_DASH_2, S_GAP_5,
  S_DASH_3, S_GAP_6,
  S_DOT_4, S_GAP_7,
  S_DOT_5, S_GAP_8,
  S_DOT_6, S_GAP_9,
  S_WORD_GAP
};

SOSState state = S_DOT_1;
unsigned long previousMillis = 0;
bool ledOn = false;

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();
  
  switch(state) {
    case S_DOT_1:  flash(DOT, S_GAP_1); break;
    case S_GAP_1:  wait(SYMBOL_GAP, S_DOT_2); break;
    case S_DOT_2:  flash(DOT, S_GAP_2); break;
    case S_GAP_2:  wait(SYMBOL_GAP, S_DOT_3); break;
    case S_DOT_3:  flash(DOT, S_GAP_3); break;
    case S_GAP_3:  wait(LETTER_GAP, S_DASH_1); break;
    case S_DASH_1: flash(DASH, S_GAP_4); break;
    case S_GAP_4:  wait(SYMBOL_GAP, S_DASH_2); break;
    case S_DASH_2: flash(DASH, S_GAP_5); break;
    case S_GAP_5:  wait(SYMBOL_GAP, S_DASH_3); break;
    case S_DASH_3: flash(DASH, S_GAP_6); break;
    case S_GAP_6:  wait(LETTER_GAP, S_DOT_4); break;
    case S_DOT_4:  flash(DOT, S_GAP_7); break;
    case S_GAP_7:  wait(SYMBOL_GAP, S_DOT_5); break;
    case S_DOT_5:  flash(DOT, S_GAP_8); break;
    case S_GAP_8:  wait(SYMBOL_GAP, S_DOT_6); break;
    case S_DOT_6:  flash(DOT, S_GAP_9); break;
    case S_GAP_9:  wait(SYMBOL_GAP, S_WORD_GAP); break;
    case S_WORD_GAP: wait(WORD_GAP, S_DOT_1); break;
  }
}

void flash(int duration, SOSState nextState) {
  if (!ledOn) {
    digitalWrite(LED_PIN, HIGH);
    ledOn = true;
    previousMillis = millis();
  }
  if (millis() - previousMillis >= duration) {
    digitalWrite(LED_PIN, LOW);
    ledOn = false;
    state = nextState;
    previousMillis = millis();
  }
}

void wait(int duration, SOSState nextState) {
  if (millis() - previousMillis >= duration) {
    state = nextState;
    previousMillis = millis();
  }
}