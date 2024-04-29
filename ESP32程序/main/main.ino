#include <Arduino.h>
#include <stdint.h>

// 定义PWM输出的引脚
#define PWM_PIN 13
// 定义方向信息的引脚
#define DIR_PIN 14
// 定义命令码字
#define MOVE_STAGE 11
#define DIR_POSITIVE 12
#define DIR_NEGATIVE 13

hw_timer_t *timer = NULL;

volatile uint32_t steps_cur = 0;

void IRAM_ATTR onTimer() {
  static unsigned int cnt = 0;
  if (steps_cur != 0) {
    steps_cur--;
    cnt++;
    digitalWrite(PWM_PIN, cnt % 2);
    // digitalWrite(DIR_PIN, cnt % 2);
  }
}

void setup() {
  Serial.begin(115200);
  timer = timerBegin(0, 80, true); // Timer 0, prescaler of 80, count up
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 5, true); // 5us interval
  timerAlarmEnable(timer);

  pinMode(PWM_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    String command = "";
    while (Serial.available() > 0) {
      char c = Serial.read();
      if (c == '\n') {
        processCommand(command);
        command = "";
      } else {
        command += c;
      }
    }
  }
  delay(50);
}

void processCommand(String command) {
  int cmd = command.toInt();
  switch (cmd) {
    case MOVE_STAGE: {
      int commaIndex1 = command.indexOf(',', 0);
      int commaIndex2 = command.indexOf(',', commaIndex1 + 1);
      if (commaIndex1 > 0 && commaIndex2 > 0) {
        int frequency = command.substring(commaIndex1 + 1, commaIndex2).toInt();
        int cycles = command.substring(commaIndex2 + 1).toInt();
        setPWMFrequency(frequency, cycles);
      }
      break;
    }
    case DIR_POSITIVE:
      digitalWrite(DIR_PIN, HIGH);
      break;
    case DIR_NEGATIVE:
      digitalWrite(DIR_PIN, LOW);
      break;
  }
}

void setPWMFrequency(unsigned int frequency, unsigned int cycles) {
  unsigned int interval = (1000000 / frequency) / 2; // Calculate half period in microseconds
  steps_cur = cycles * 2; // Each cycle consists of an ON and OFF, hence *2
  timerAlarmWrite(timer, interval, true);
}
