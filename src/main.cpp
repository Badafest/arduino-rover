#include <Arduino.h>
#include <AFMotor.h>

AF_DCMotor leftMotor(1);
AF_DCMotor rightMotor(2);

const uint8_t trigPin = A3;                // US trig pin
const uint8_t echoPin = A4;                // US echo pin
const uint8_t MOTOR_SPEED = 128;           // 0 - 255
const uint8_t MIN_CLEARANCE = 30;          // cm
const unsigned long CHECK_FREQUENCY = 100; // ms

float distance;

bool motorsRunning = false;
unsigned long lastChecked = 0;

void setup()
{
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  lastChecked = millis() - CHECK_FREQUENCY;
}

void loop()
{
  unsigned long now = millis();

  if (now - lastChecked > CHECK_FREQUENCY)
  {

    lastChecked = now;

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    distance = (pulseIn(echoPin, HIGH) * .0343) / 2;
  }

  if (distance > MIN_CLEARANCE && !motorsRunning)
  {
    leftMotor.setSpeed(MOTOR_SPEED);
    rightMotor.setSpeed(MOTOR_SPEED);

    leftMotor.run(FORWARD);
    rightMotor.run(FORWARD);

    motorsRunning = true;
  }
  else if (motorsRunning)
  {
    leftMotor.setSpeed(0);
    rightMotor.setSpeed(0);

    leftMotor.run(RELEASE);
    rightMotor.run(RELEASE);

    motorsRunning = false;
  }
}