#include "LiquidCrystal_I2C.h"
LiquidCrystal_I2C lcd(0x27, 16, 2);

int buttons[4] = {2, 3, 4, 5};
int leds[4] = {8, 9, 10, 11};

int Init = 7;
boolean Solved;
int Failed = 13;

boolean button[4] = {0, 0, 0, 0};
byte o_acentuada[8] = {
  B00010,
  B00100,
  B00000,
  B01110,
  B10001,
  B10001,
  B10001,
  B01110
};

#define buzzer 6
#define levelsInGame 5

int bt_simonSaid[100];
int led_simonSaid[100];

boolean lost;
int game_play, level, etapa;

void setup() {
  Serial.begin(9600);
  
  for (int i = 0; i <= 3; i++) {
    pinMode(buttons[i], INPUT_PULLUP);
    pinMode(leds[i], OUTPUT);
  }

  pinMode(buzzer, OUTPUT);
  pinMode(Init, INPUT);
  pinMode(Solved, OUTPUT);
  pinMode(Failed, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.createChar(0, o_acentuada); // Crear el carácter personalizado
  lcd.setCursor(0, 0);
  lcd.print(" Memoria ");
  lcd.setCursor(0, 1);
  lcd.print(" Musical ");
  delay(2000);
  lcd.clear();
  Solved = false;
  randomSeed(analogRead(0));
}

void loop() {
  lost = false;
  if (Solved == false) {
    botonesMusicales();
  }
}

void botonesMusicales() {
  switch (etapa) {
    case 0:
      lcd.setCursor(0, 0); lcd.print("Oprime bot");
      lcd.write(byte(0));  // Imprime la "ó" personalizada
      lcd.print("n rojo para iniciar");
      button[0] = digitalRead(buttons[0]);
      while(button[0] == HIGH) {
        button[0] = digitalRead(buttons[0]);
      }
      level = 1;
      game_play = 1;
      etapa = 1;  // Actualizar la variable global etapa
      break;

    case 1:
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("Nivel: ");
      lcd.print(level);
      lcd.setCursor(0, 1);
      lcd.print(" -- Memoriza -- ");
      delay(1500);
      led_simonSaid[level] = random(8, 12);
      for (int i = 1; i <= level; i++) {
        digitalWrite(led_simonSaid[i], HIGH);
        playBuzzer(led_simonSaid[i] - 7);
        digitalWrite(led_simonSaid[i], LOW);
        delay(400);
      }
      delay(500);
      etapa = 2;  // Actualizar la variable global etapa
      break;

    case 2:
      etapa = 3;  // Actualizar la variable global etapa
      lcd.setCursor(0, 1);
      lcd.print("  -- Juega --  ");
      break;

    case 3:
      for (int i = 0; i <= 3; i++) {
        button[i] = digitalRead(buttons[i]);
        if (button[i] == LOW) {
          bt_simonSaid[game_play] = leds[i];
          digitalWrite(leds[i], HIGH);
          playBuzzer(i + 1);
          while (button[i] == LOW) {
            button[i] = digitalRead(buttons[i]);
          }
          delay(50);
          digitalWrite(leds[i], LOW);
          if (led_simonSaid[game_play] != leds[i]) {
            lost = true;
            etapa = 5;  // Actualizar la variable global etapa
            break;
          }
          game_play++;
          if (game_play - 1 == level) {
            game_play = 1;
            etapa = 4;  // Actualizar la variable global etapa
            break;
          }
        }
      }
      delay(10);
      break;

    case 4:
      lcd.setCursor(0, 1);
      lcd.print("  Comprobando  ");
      delay(1000);
      if (!lost) {
        if (level == levelsInGame) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("   Juego   ");
          lcd.setCursor(0, 1);
          lcd.print("  Completado  ");
          delay(1000);
          level = 1;
          etapa = 1;  // Actualizar la variable global etapa
          digitalWrite(Solved, HIGH);
        } else {
          if (level < levelsInGame) level++;
          etapa = 1;  // Actualizar la variable global etapa
        }
      }
      break;

    case 5:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("    PERDISTE   ");
      tone(buzzer, 350);
      for (int i = 0; i <= 3; i++) {
        digitalWrite(leds[i], HIGH);
      }
      delay(1000);
      noTone(buzzer);
      delay(1000);
      for (int i = 0; i <= 3; i++) {
        digitalWrite(leds[i], LOW);
      }
      digitalWrite(Failed, HIGH);
      delay(100);
      digitalWrite(Failed, LOW);
      etapa = 0;  // Actualizar la variable global etapa
      lost = false;
      lcd.clear();
      break;

    default:
      break;
  }
}

void playBuzzer(int x) {
  tone(buzzer, 650 + (x * 100));
  delay(300);
  noTone(buzzer);
}
