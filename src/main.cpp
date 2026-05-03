#include <Arduino.h>
#include <AFMotor.h>

AF_DCMotor leftMotor(1);
AF_DCMotor rightMotor(2);

const uint8_t irPin = A2;                   // IR sensor pin
const uint8_t trigPin = A3;                 // US trig pin
const uint8_t echoPin = A4;                 // US echo pin
const uint8_t MOTOR_SPEED = 125;            // 0 - 255
const uint8_t MIN_CLEARANCE = 20;           // cm
const unsigned long CHECK_INTERVAL = 60;    // ms
const unsigned long RECOVER_INTERVAL = 100; // ms

float distance;

bool motorsRunning = false;
unsigned long obstacleCheckTimestamp = 0;
bool floorPresent = false;

void setup()
{
  Serial.begin(9600);

  pinMode(irPin, INPUT);
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);

  obstacleCheckTimestamp = millis() - 2 * CHECK_INTERVAL;

  leftMotor.setSpeed(MOTOR_SPEED);
  rightMotor.setSpeed(MOTOR_SPEED);

  leftMotor.run(FORWARD);
  rightMotor.run(FORWARD);
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

    Serial.print("distance: ");
    Serial.println(distance);
  }

  floorPresent = digitalRead(irPin) == LOW;

  if (distance > MIN_CLEARANCE && floorPresent)
  {
    if (motorsRunning)
    {
      return;
    }

    // GO
    leftMotor.setSpeed(MOTOR_SPEED);
    rightMotor.setSpeed(MOTOR_SPEED);

    leftMotor.run(FORWARD);
    rightMotor.run(FORWARD);

    motorsRunning = true;
  }
  else
  {
    if (!motorsRunning)
    {
      return;
    }

    if (!floorPresent)
    {
      // BREAK!!
      leftMotor.setSpeed(MOTOR_SPEED * 2);
      rightMotor.setSpeed(MOTOR_SPEED * 2);

      leftMotor.run(BACKWARD);
      rightMotor.run(BACKWARD);

      delay(RECOVER_INTERVAL);
    }

    // STOP
    leftMotor.setSpeed(0);
    rightMotor.setSpeed(0);

    leftMotor.run(RELEASE);
    rightMotor.run(RELEASE);

    motorsRunning = false;
  }
}