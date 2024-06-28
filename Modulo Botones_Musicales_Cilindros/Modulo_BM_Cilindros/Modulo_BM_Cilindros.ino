//Definicion Variables Cilindros
const int pinFirstButton = 2; // Primer boton
const int numButtons = 5;
const int pinLed = 7;
const int seqLen = 5; // Cada boton se presiona 1 vez
int inputSequence[seqLen];
//orden en que se deben presionar los botones numerados del 1 al 5
int correctSequence[seqLen]={1,2,3,4,5};
// tiempo maximo permitido entre pasos de la secuencia
const int timeout = 5000;
int currentPos=0;
unsigned long lastButtonPressTime=0;

const int badAnswerPin = 10; //pin para enviar señal de solucion incorrecta (no usado)
const int cerebroApproval = 11; //pin para activar el modulo
//bool gameSolved = false; //pin para enviar cuando se solucione correctamente
bool gameWon=false;
//Fin Variables Cilindros
//Definicion Variables BM
#include "LiquidCrystal_I2C.h"
LiquidCrystal_I2C lcd(0x27, 16, 2);

int buttons[4] = {2, 3, 4, 5};
int leds[4] = {8, 9, 10, 11};

//int Init = 7; //Cerebro approval
int Solved=8; //Game won
//int Failed = 13; //Bad Answer

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

boolean lost; //verificacion por nivel
int game_play, level, etapa;

//Fin variables BM

void setup() {
  Serial.begin(9600);
  
  for (int i = 0; i <= 3; i++) {
    pinMode(buttons[i], INPUT); //necesario resistencia pullup
    pinMode(leds[i], OUTPUT);
  }
  
   for (int i=0; i < numButtons; i++){
     pinMode(pinFirstButton + i, INPUT);
   }
  pinMode(pinLed, OUTPUT);
  pinMode(badAnswerPin, OUTPUT);
  pinMode(cerebroApproval, INPUT);
  //pinMode(gameSolved, OUTPUT);
  
  pinMode(buzzer, OUTPUT);
  //pinMode(Init, INPUT);
  pinMode(Solved, OUTPUT);
  
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, o_acentuada); // Crear el carácter personalizado
  lcd.setCursor(0, 0);
  lcd.print(" Memoria ");
  lcd.setCursor(0, 1);
  lcd.print(" Musical ");
  delay(2000);
  lcd.clear();
  digitalWrite(Solved,LOW);
  randomSeed(analogRead(0));
}

void loop() {
  if (digitalRead(cerebroApproval) == HIGH){
   if (gameWon==false){
      unsigned long currentTime = millis();
  
     if (currentPos > 0 && ((currentTime - lastButtonPressTime) > timeout)) {
        Serial.println("Timeout! Sequence reset."); //debug
        resetSequence();
        wrongSolution();
      }
      
    for (int i=0; i < numButtons; i++){
      if (digitalRead(pinFirstButton+i)){
        delay(50);
        Serial.print("Boton ");//debug
        Serial.print(i+1);//debug
        Serial.println(" encendido");//debug
        delay(100);
        //digitalWrite(pinLed+i, HIGH);//debug
        inputSequence[currentPos] = i+1;
        lastButtonPressTime = currentTime;
        currentPos++;
        printSequence(currentPos); //debug
        while(digitalRead(pinFirstButton+i)){
          delay(100);
          Serial.println("Waiting for release...");//debug
        }
        
        if (currentPos == seqLen) {
            if (isCorrectSequence()) {
              Serial.println("Correct sequence entered!");//debug
              //digitalWrite(gameSolved, HIGH);
              gameWon=true;
            } 
            else {
              Serial.println("Incorrect sequence. Try again."); //debug
              wrongSolution();
            }
            resetSequence();
          }
      }//if high
    }//for
          
          
   }
    else{
        int blinkDelay = 200;
        digitalWrite(pinLed, HIGH); // LED ON
        delay(blinkDelay);
        digitalWrite(pinLed, LOW); // LED OFF
        delay(blinkDelay);         
    }
    //}


  
  lost = false;
  if (digitalRead(Solved) == LOW && gameWon == true) {
    botonesMusicales();
  }
}
//else if(digitalRead(Solved)==HIGH){} rutina fin de juego

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
      digitalWrite(badAnswerPin, HIGH);
      delay(100);
      digitalWrite(badAnswerPin, LOW);
      etapa = 0;  // Actualizar la variable global etapa
      lost = false;
      lcd.clear();
      break;

    default:
      break;
  }
}

void resetSequence(){
  currentPos = 0;
  lastButtonPressTime = millis();
  for (int i = 0; i < seqLen; i++) {
    inputSequence[i] = -1; //llena con -1
  }
  Serial.println("Sequence reset."); //debug

}
               
               
void wrongSolution(){
  int blinkTimes = 5;
  int blinkDelay = 500;
  digitalWrite(badAnswerPin, HIGH); //enviar solucion incorrecta
  for (int i=0; i < blinkTimes; i++){
    digitalWrite(pinLed,HIGH); // LED ON
    delay(blinkDelay);
    digitalWrite(pinLed, LOW); // LED OFF
    delay(blinkDelay);  
   }
  digitalWrite(badAnswerPin, LOW); //reset
}                    

bool isCorrectSequence(){
  for (int i = 0; i < seqLen; i++) {
      if (inputSequence[i] != correctSequence[i]) {
          return false;
      }
    }
  return true;
}

void printSequence(int position){
  for (int i=0; i<position; i++){
    Serial.print(inputSequence[i]);
    Serial.print(", ");
  }
  Serial.println(".");
}


void playBuzzer(int x) {
  tone(buzzer, 650 + (x * 100));
  delay(300);
  noTone(buzzer);
}
