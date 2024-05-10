#include <Arduino.h>

#define MOTOR_FORWARD 3
#define MOTOR_BACKWARD 2

void setup() {
  pinMode(MOTOR_BACKWARD, OUTPUT);
  pinMode(MOTOR_FORWARD, OUTPUT);
}

int pwm = 100;

void loop() {
  analogWrite(MOTOR_BACKWARD, 0);
  analogWrite(MOTOR_FORWARD, pwm);
  delay(1000);
  analogWrite(MOTOR_FORWARD, 0);
  delay(1000);
  analogWrite(MOTOR_FORWARD, 0);
  analogWrite(MOTOR_BACKWARD, pwm);
  delay(1000);
  analogWrite(MOTOR_BACKWARD, 0);
  delay(1000);
}
