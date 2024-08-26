#include <Keypad.h>
#include <Adafruit_NeoPixel.h>

// Define the number of rows and columns for the keypad
const byte ROWS = 3;
const byte COLS = 3;
#define cornerLEDPin 2

bool initialCondition[ROWS][COLS] = {{1,1,1} , {0,0,0}, {0,0,0}}; //Estado en el que inician los LEDs
bool LEDStates[ROWS][COLS] = {}; //Estado actual de los LEDs
bool desiredStates[ROWS][COLS] = {{0,1,0} , {0,0,1}, {1,0,1}}; //Respuesta del juego LEDs

char keyPad [ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'}
};

byte rowPins[ROWS] = {A0, A1, A2};  // Pines conectados a las filas de los botones
byte colPins[COLS] = {A3, A4, A5};  // Pines conectados a las columnas de los botones

byte ledRowPins[ROWS] = {2, 3, 4};  // Pines conectados a las FILAS de la matriz de LEDs, filas VCC
byte ledColPins[COLS] = {5, 6, 7};  //Pines conectados a las COLUMNAS de la matriz de LEDs, columnas GND

Keypad buttonMatrix = Keypad(makeKeymap(keyPad), rowPins, colPins, ROWS, COLS);
Adafruit_NeoPixel cornerLED = Adafruit_NeoPixel(1, cornerLEDPin, NEO_GRB + NEO_KHZ800);
//Variables para la comunicacion con el cerebro
#define gameSolved 8
#define badAnswerPin 9
#define cerebroApproval 10
#define roadStart 11
bool gameWon = false;

//Simbolos fin del juego
unsigned long timePerSymbol = 2500; //tiempo que dura cada simbolo en ms
unsigned long symbolStartTime = 0; //tiempo en el que el simbolo inicio a mostrarse
unsigned long timeSinceGameWon = 5000;
bool firstSymbol[ROWS][COLS] = {{1,1,1} , {0,0,1}, {0,0,1}};
bool secondSymbol[ROWS][COLS] = {{1,0,1} , {0,1,1}, {1,1,0}};
bool thirdSymbol[ROWS][COLS] = {{0,0,1} , {0,0,0}, {1,0,0}};
bool fourthSymbol[ROWS][COLS] = {{1,1,0} , {0,0,1}, {1,1,0}};
int currentSymbolIndex = 0;

bool blackOutDone = false;

void setup(){
  // Initialize serial communication
  Serial.begin(9600);
  cornerLED.begin();
  cornerLED.show();

  // Initialize LED states with the initial conditions
  for (int i = 0; i < ROWS; i++){
    for (int j = 0; j < COLS; j++){
      LEDStates[i][j] = initialCondition[i][j];
    }
  }
  for (int i = 0; i < ROWS; i++){
    pinMode(ledRowPins[i], OUTPUT);
    digitalWrite(ledRowPins[i], LOW);  // Inicializa las filas en LOW (anodo GND)
  }
  for (int i = 0; i < COLS; i++){
    pinMode(ledColPins[i], OUTPUT);
    digitalWrite(ledColPins[i], HIGH);  // Inicializa las columnas en HIGH (cathode flotando)
  }
  pinMode(badAnswerPin, OUTPUT);
  pinMode(cerebroApproval, INPUT);
  pinMode(gameSolved, OUTPUT);
  pinMode(roadStart, OUTPUT);
  digitalWrite(roadStart, HIGH); //es inicio de camino
}

void loop(){
  if (digitalRead(cerebroApproval) == HIGH){
    if (!gameWon){
      char button = buttonMatrix.getKey();
      cornerLED.setPixelColor(0,cornerLED.Color(0,255,0)); //verde
      cornerLED.show();
      if (button){
        buttonPressed(button);
      }
      updateLEDMatrix(); //enciende los LEDs rapidamente
    }
    else{
      digitalWrite(gameSolved, HIGH);
      cornerLED.setPixelColor(0,cornerLED.Color(127,0,255)); //morado
      cornerLED.show();
      //simbolos fin de juego
      unsigned long currentTime = millis();
      static unsigned long gameWonTime = 0; 
      if (gameWonTime == 0){  // se ejecuta la primera vez
        gameWonTime = currentTime;  // Momento en el que el juego se completa
      }
      
      /*if (currentTime - gameWonTime < timeSinceGameWon){
        updateLEDMatrix();  // Se mantiene encendido los LEDs en la respuesta correcta durante el tiempo
      }*/
      else if(currentTime - gameWonTime < 2*timeSinceGameWon){
        //Despues se apagan los LEDs por el mismo tiempo que duraron encendidos
        for (int i = 0; i < ROWS; i++){
          for (int j = 0; j < COLS; j++){
            LEDStates[i][j] = 0; //apagarlos
          }
        }
      }
      
      else { //repite hasta que se cambie la orden del cerebro
        unsigned long timeSinceSymbolStart = currentTime - symbolStartTime;
        if (timeSinceSymbolStart >= timePerSymbol) {  // Es momento de cambiar al siguiente simbolo
          currentSymbolIndex = (currentSymbolIndex + 1) % 4;  // Cicla entre los 4 simbolos
          symbolStartTime = currentTime;  // Resetea el tiempo en el que se inicia el simbolo
          displaySymbol(currentSymbolIndex);
        }
        
      }
      // Se mantiene encendido los LEDs en la respuesta correcta durante el timeSinceWon,
      // luego se dejan apagados durante timeSinceWon
      // el resto del tiempo cicla entre simbolos
      updateLEDMatrix();
    }
  }
  //blackout
  else{ //se dio la orden de apagarse
    if (gameWon){
      pinMode(roadStart, INPUT);
      if (digitalRead(roadStart) == HIGH){
        blackOut();
      }
    }
  }
}

void buttonPressed(char button){
  int row = -1, col = -1;

  // Encuentra la fila y columna en la que fue presionado el boton
  for (int i = 0; i < ROWS; i++){
    for (int j = 0; j < COLS; j++){
      if (keyPad[i][j] == button){
        row = i;
        col = j;
        break;  // Sale del loop al encontrar el boton presionado
      }
    }
  }
  // Si se presiono un boton valido
  if (row != -1 && col != -1){
    toggleLEDs(row, col);  // Cambiar de estado los LEDs basado en el boton presionado
  }
}
void toggleLEDs(int row, int col){
  LEDStates[row][col] = !LEDStates[row][col]; //Cambia de estado el LED del boton presionado (i,j)
  
  if (col > 0){ //Si no esta en la primera columna
    LEDStates[row][col - 1] = !LEDStates[row][col - 1]; //Cambia de estado el LED (i, j-1)
  }
  if (col < COLS - 1){ //Si no esta en la ultima columna
    LEDStates[row][col + 1] = !LEDStates[row][col + 1]; //Cambia de estado el LED (i, j+1)
  }
  // Si no esta en la primera fila
  if (row > 0){
    LEDStates[row - 1][col] = !LEDStates[row - 1][col]; //Cambia de estado el LED (i-1, j)
  }
  // Si no esta en la ultima fila
  if (row < ROWS - 1){
    LEDStates[row + 1][col] = !LEDStates[row + 1][col];
  }
  //una vez se actualizan todos los LEDs
  checkGameSolved();
}

void updateLEDMatrix(){
  //recorre los estados de toda la matriz
  // para encender un LED especifico, se envia el voltaje a la FILA y 0 a la COLUMNA
  // para evitar que otros se enciendan, se envia 0 a la FILA y voltaje a la COLUMNA
  for (int row = 0; row < ROWS; row++){
    for (int col = 0; col < COLS; col++){
      if (LEDStates[row][col]){  // Si el LED deberia estar encendido
        //Polarizar el LED
        digitalWrite(ledRowPins[row], HIGH);  // Anodo en HIGH
        digitalWrite(ledColPins[col], LOW);  // Catodo en  LOW
        delayMicroseconds(1000);  // Delay for LED to be visible (adjust as needed)
        //Quitar la polarizacion
        digitalWrite(ledRowPins[row], LOW);  // Anodo en LOW
        digitalWrite(ledColPins[col], HIGH);  // Catodo en HIGH
      }
    }
  }
}

void checkGameSolved(){
  gameWon = true;  // Se asume el juego como completado
  // Se revisan todos los estados de los LEDs
  for (int row = 0; row < ROWS; row++){
    for (int col = 0; col < COLS; col++){
      if (LEDStates[row][col] != desiredStates[row][col]){
        gameWon = false;  // Si alguno de los LEDs es diferente, no se completo
        return;
      }
    }
  }
}
void displaySymbol(int symbolIndex) {
  bool (*currentSymbol)[COLS];  // Apuntador al arreglo de simbolos

  // Asignar el simbolo segun el indice
  switch (symbolIndex) {
    case 0:
      currentSymbol = firstSymbol;
      break;
    case 1:
      currentSymbol = secondSymbol;
      break;
    case 2:
      currentSymbol = thirdSymbol;
      break;
    case 3:
      currentSymbol = fourthSymbol;
      break;
    default:
      return;  //  indice invalido
  }
  for (int i = 0; i < ROWS; i++){
    for (int j = 0; j < COLS; j++){
      LEDStates[i][j] = currentSymbol[i][j]; //asignarle el simbolo
    }
  }
}
  
void blackOut(){
  if (!blackOutDone){
    for (int i = 0; i < ROWS; i++){
      for (int j = 0; j < COLS; j++){
        LEDStates[i][j] = 0; //apagarlos
      }
    }
    updateLEDMatrix();
    cornerLED.setPixelColor(0,cornerLED.Color(0,0,0)); //apagado
    cornerLED.show();
  }
}
