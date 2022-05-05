#include <PinChangeInterrupt.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <TimerOne.h>

#define trigPin 7
#define echoPin 6
#define BUZZER 5
#define BUTTON 4
#define LED 13

//*********SET THE DISTANCES BELOW WHICH THE BUZZER WILL GO OFF*********

float firstDistance = 20.00;
float secondDistance = 50.00;

//*********************************************************************

LiquidCrystal_I2C lcd(0x27, 20, 4);

int ledState = 0;
int buzzerState = 0;
bool startMeasure = false;
unsigned long buzzerTime = 250000UL;

void setup() {
  //setting pinModes
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(BUTTON, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);

  //changing the state of a button triggers the startSensor() function
  attachPCINT(digitalPinToPCINT(BUTTON), startSensor, FALLING);

  //setting the Timer1 frequency and the function that it triggers
  Timer1.initialize(buzzerTime);
  Timer1.attachInterrupt(buzzer_led_give_sign);

  //initalizing the LCD
  lcd.init();
}

void buzzer_led_give_sign() {
  if ((buzzerState == 0) && (measure() < firstDistance) && (startMeasure)) {
    buzzerState = 50;
    ledState = 1;
    buzzerTime = 250000UL;
  } else if  ((buzzerState == 0) && (measure() < secondDistance) && (startMeasure)) {
    buzzerState = 100;
    ledState = 0;
    buzzerTime = 500000UL;
  } else {
    buzzerState = 0;
    ledState = 0;
  }
  Timer1.setPeriod(buzzerTime);
  analogWrite(BUZZER, buzzerState);
}

void startSensor() {
  startMeasure = !startMeasure;
}

float measure() {
  unsigned long timeOfMeasure;
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  timeOfMeasure = pulseIn(echoPin, HIGH);
  return timeOfMeasure / 58.00;
}

void loop() {
  //stop the vehicle
  digitalWrite(LED, ledState);
  //messages on the LCD
  if (startMeasure) {
    if (measure() >= 10)
    {
      lcd.backlight();
      lcd.setCursor(1, 0);
      lcd.print("DISTANCE: ");
      lcd.setCursor(1, 1);
      lcd.print(measure(), 3);
      lcd.setCursor(7, 1);
      lcd.print("cm      ");
    }
    else
    {
      lcd.backlight();
      lcd.setCursor(1, 0);
      lcd.print("DISTANCE: ");
      lcd.setCursor(1, 1);
      lcd.print(measure(), 3);
      lcd.setCursor(6, 1);
      lcd.print("cm      ");
    }
  }
  else {
    lcd.backlight();
    lcd.setCursor(1, 0);
    lcd.print("POWER: OFF");
    lcd.setCursor(1, 1);
    lcd.print("PRESS TO START");
  }
  delay(500);
}
