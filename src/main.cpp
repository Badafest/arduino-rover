#include <Arduino.h>
#include <AFMotor.h>

AF_DCMotor leftMotor(1);
AF_DCMotor rightMotor(2);

const uint8_t floorPin = A1;               // fllor indicator pin
const uint8_t irPin = 2;                   // IR sensor pin
const uint8_t trigPin = A3;                // US trig pin
const uint8_t echoPin = A4;                // US echo pin
const uint8_t obstaclePin = A5;            // obstacle indicator pin
const uint8_t MOTOR_SPEED = 125;           // 0 - 255
const uint8_t MIN_CLEARANCE = 20;          // cm
const unsigned long CHECK_INTERVAL = 60;   // ms
const unsigned long RECOVER_DELAY = 150;   // ms
const unsigned long BACK_AWAY_DELAY = 500; // ms

float distance;

bool motorsRunning = false;
unsigned long obstacleCheckTimestamp = 0;
bool floorPresent = false;

void setup()
{
  pinMode(floorPin, OUTPUT);
  pinMode(irPin, INPUT);
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(obstaclePin, OUTPUT);

  obstacleCheckTimestamp = millis() - 2 * CHECK_INTERVAL;
}

void loop()
{
  unsigned long now = millis();

  if (now - obstacleCheckTimestamp > CHECK_INTERVAL)
  {
    obstacleCheckTimestamp = now;

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    distance = (pulseIn(echoPin, HIGH) * .0343) / 2;
  }

  floorPresent = digitalRead(irPin) == LOW;

  digitalWrite(obstaclePin, distance > MIN_CLEARANCE ? HIGH : LOW);
  digitalWrite(floorPin, floorPresent ? LOW : HIGH);

  if (distance > MIN_CLEARANCE && floorPresent)
  {
    if (motorsRunning)
    {
      return;
    }

    // GO
    leftMotor.setSpeed(MOTOR_SPEED);
    leftMotor.run(FORWARD);

    rightMotor.setSpeed(MOTOR_SPEED);
    rightMotor.run(FORWARD);

    motorsRunning = true;

    return;
  }

  if (!floorPresent && motorsRunning)
  {
    // BREAK!!
    leftMotor.setSpeed(MOTOR_SPEED * 2);
    leftMotor.run(BACKWARD);

    rightMotor.setSpeed(MOTOR_SPEED * 2);
    rightMotor.run(BACKWARD);

    delay(RECOVER_DELAY);
  }

  if (motorsRunning)
  {
    // STOP
    leftMotor.setSpeed(0);
    rightMotor.run(RELEASE);

    rightMotor.setSpeed(0);
    rightMotor.run(RELEASE);
  }

  if (floorPresent)
  {
    // BACK AWAY
    leftMotor.setSpeed(MOTOR_SPEED);
    leftMotor.run(BACKWARD);

    delay(BACK_AWAY_DELAY);
  }

  motorsRunning = false;
}