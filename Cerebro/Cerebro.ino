#include "LiquidCrystal_I2C.h"
#include "Countimer.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define startTimer 2
#define pauseTimer 3
#define restartTimer 4
#define stopTimer 5

#define Init 6
#define Solved 7
#define Failed 8

Countimer tDown;
Countimer tNone;

void setup() {
  pinMode(startTimer, INPUT_PULLUP);
  pinMode(pauseTimer, INPUT_PULLUP);
  pinMode(restartTimer, INPUT_PULLUP);
  pinMode(stopTimer, INPUT_PULLUP);
  
  pinMode(Init, OUTPUT);
  pinMode(Solved, INPUT);
  pinMode(Failed, INPUT);
  
  Serial.begin(9600);
  digitalWrite(13, LOW);

  tDown.setCounter(0, 30, 0, tDown.COUNT_DOWN, tDownComplete);
  tDown.setInterval(print_time2, 1000);
  tNone.setInterval(print_none, 2000);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Countdown  Timer");
}

void loop() {
  tDown.run();
  tNone.run();

  if (digitalRead(startTimer) == LOW) {
    digitalWrite(Init, HIGH);
    delay(100);
    digitalWrite(Init, LOW);
    tDown.start();
    tNone.start();
  }
  
  if (digitalRead(pauseTimer) == LOW) {
    tDown.pause();
    tNone.pause();
  }
  
  if (digitalRead(restartTimer) == LOW) {
    tDown.restart();
    tNone.restart();
  }
  
  if (digitalRead(stopTimer) == LOW) {
    tDown.stop();
    tNone.stop();
  }

  if (digitalRead(Solved) == HIGH) {
    tDown.stop();
    tNone.stop();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Game Completed!");
  }
  
  if (digitalRead(Failed) == LOW) {
    tDown.pause();
    tDown.setCounter(tDown.getCurrentHours(), tDown.getCurrentMinutes(), tDown.getCurrentSeconds()-30, tDown.COUNT_DOWN, tDownComplete);
    tDown.start();
  }
}

void print_time2() {
  Serial.print("tDown: ");
  lcd.setCursor(4, 1);
  lcd.print(tDown.getCurrentTime());
}

void print_none() {
  Serial.print("tNone: millis(): ");
  Serial.println(millis());
}

void tDownComplete() {
  digitalWrite(13, HIGH);
}
