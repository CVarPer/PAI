#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal_I2C.h>

//Definicion Variables Cilindros

const int numButtons = 5;
const int pinCilinderButtons [numButtons] = {2,3,4,5,6}; // Primer boton
const int seqLen = 5; // Si es igual al numero de botones, cada boton se presiona 1 vez
int inputSequence[seqLen];

//orden en que se deben presionar los botones numerados del 1 al 5
int correctSequence[seqLen]={1,2,3,4,5}; //respuesta a modificar segun go training
// tiempo maximo permitido entre pasos de la secuencia
const int timeOutTime = 5000;
int currentPos = 0;
unsigned long lastButtonPressTime=0;

#define gameSolved 8 //pin para enviar cuando se solucione correctamente
#define badAnswerPin 9 //pin para enviar señal de solucion incorrecta
#define cerebroApproval 10 //pin para activar el modulo
#define roadStart 11
bool gameWon=false; // modulo completado
bool completedCilinders = false; //Fin de Cilindros

unsigned long timeForMistakeSignal = 3000; //Tiempo durante el que se envia la señal de error
unsigned long errorStartTime = 0;
bool errorCommited = false;

#define cornerLEDPin 12
Adafruit_NeoPixel cornerLED = Adafruit_NeoPixel(1, cornerLEDPin, NEO_GRB + NEO_KHZ800);

#define LEDsRowPin 7
#define numLEDsInRow 7
Adafruit_NeoPixel LEDsRow = Adafruit_NeoPixel(numLEDsInRow, LEDsRowPin, NEO_GRB + NEO_KHZ800);

//Definicion Variables BM

LiquidCrystal_I2C lcd(0x27, 16, 2);
const int numMusicalButtons = 4;
int musicalButtons[numMusicalButtons] = {2, 3, 4, 5};
int leds[numMusicalButtons] = {8, 9, 10, 11};

boolean button[numMusicalButtons] = {0, 0, 0, 0};
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

#define buzzerPin 6
#define levelsInGame 5

int bt_simonSaid[100];
int led_simonSaid[100];

const int sequenceIncrement[levelsInGame] = {3, 3, 4, 4, 4}; //Numero de botones adicionales por nivel
boolean levelLost = false; //verificacion por nivel
int game_play, level, etapa;
unsigned long lastActionTime = 0;  // Ultima vez que se presiono un boton
const unsigned long timeoutDuration = 10000;  // 10 segundos para timeout

bool blackOutDone = false;
//fin del juego
unsigned long lastPatternUpdate = 0;
const unsigned long patternUpdateInterval = 100;  
unsigned long lastBlinkTime = 0;
const unsigned long blinkInterval = 500;  
bool buttonLEDState = false;  //Estado de los LEDs de botones musicales
int rainbowIndex = 0;

void setup(){
    Serial.begin(9600);
    
    for (int i = 0; i < numMusicalButtons; i++){
      pinMode(musicalButtons[i], INPUT); //necesario resistencia pulldown
    }
  
   for (int i=0; i < numButtons; i++){
     pinMode(pinCilinderButtons[i], INPUT);
   }
   //Comunicacion con el cerebro
    pinMode(badAnswerPin, OUTPUT);
    pinMode(cerebroApproval, INPUT);
    pinMode(gameSolved, OUTPUT);
    pinMode(roadStart, OUTPUT);
    digitalWrite(roadStart, LOW); // No es inicio de camino
    digitalWrite(badAnswerPin, LOW); //inicializar
    digitalWrite(gameSolved, LOW);

    cornerLED.begin();
    cornerLED.show();
    LEDsRow.begin();
    LEDsRow.show();
    pinMode(buzzerPin, OUTPUT);
    
    lcd.init();
    lcd.backlight();
    lcd.createChar(0, o_acentuada); // Crear el carácter personalizado
    /*lcd.setCursor(0, 0);
    lcd.print(" Memoria ");
    lcd.setCursor(0, 1);
    lcd.print(" Musical ");
    delay(2000);
    lcd.clear();*/
    randomSeed(analogRead(0));
}

void loop(){
  if (digitalRead(cerebroApproval) == HIGH){
    cornerLED.setPixelColor(0,cornerLED.Color(0,255,0)); //verde
    cornerLED.show();
     if (!completedCilinders){
        checkCilinders();
        checkTimeOut();

      if (errorCommited){
        wrongSolution();
        sendError();    
      }
      else{
        chasingLightPattern();
      }
    }
    else{ //Cilindros completado, inicio Botones musicales
       if (!gameWon){
         for (int i = 0; i < numLEDsInRow; i++){ 
            //Se mantiene apagado la tira de Cilindros mientras se juega con musicales
            LEDsRow.setPixelColor(i, LEDsRow.Color(0, 0, 0));
         }
         unsigned long currentTime = millis();
         if (etapa == 3 && (currentTime - lastActionTime >= timeoutDuration)){
            // Reiniciar si se demora mucho
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Tiempo agotado!");
            delay(2000);
            lcd.clear();
            etapa = 0;  // Reset to the initial stage
            levelLost = false;
            game_play = 1;
            level = 1;
            return;
          }
         botonesMusicales();
         sendError();
       }
       else{ //rutina finalizacion de juego
          cornerLED.setPixelColor(0,cornerLED.Color(127,0,255)); //morado
          cornerLED.show();
          for (int i = 0; i < numMusicalButtons; i++){
            pinMode(musicalButtons[i], OUTPUT); //necesario resistencia pulldown
          }
          endGamePattern();
       }
    }
  }
  else{
    if (gameWon){
      pinMode(roadStart, INPUT);
      if (digitalRead(roadStart) == HIGH){
        blackOut();
      }
    }
  }
}


void botonesMusicales(){
  // LOW boton sin presionar HIGH boton presionado
  switch (etapa){
    case 0: //Presione boton para iniciar
      lcd.setCursor(0, 0); lcd.print("Oprime bot");
      lcd.write(byte(0));  // Imprime la "ó" personalizada
      lcd.print("n rojo para iniciar");
      button[0] = digitalRead(musicalButtons[0]);
      if (button[0] == HIGH) {  // Boton presionado
        while (digitalRead(musicalButtons[0]) == HIGH);  // Esperar a que se suelte
        level = 1;
        game_play = 1;
        etapa = 1;
        lastActionTime = millis();
      }
      break;

    case 1: //Generar secuencia
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("Nivel: ");
      lcd.print(level);
      lcd.setCursor(0, 1);
      lcd.print(" -- Memoriza -- ");
      delay(1500);
      for (int i = 0; i < sequenceIncrement[level - 1]; i++) {
        led_simonSaid[game_play] = random(2, 6);  
        game_play++;
      }
      
      showSequence();
      etapa = 2;
      break;

    case 2: //Previo al turno del jugador
      etapa = 3;
      lcd.setCursor(0, 1);
      lcd.print("  -- Juega --  ");
      game_play = 1;
      setButtonPinsAsInput();
      break;

    case 3: //Turno del jugador
      for (int i = 0; i < numMusicalButtons; i++){
        button[i] = digitalRead(musicalButtons[i]);
        if (button[i] == HIGH){
          bt_simonSaid[game_play] = musicalButtons[i];
          playBuzzer(i + 1);
          while (button[i] == HIGH){
            button[i] = digitalRead(musicalButtons[i]);
          }
          lastActionTime = millis();
          
          if (led_simonSaid[game_play] != musicalButtons[i]){
            levelLost = true;
            etapa = 5;  // Actualizar la variable global etapa
            errorCommited = true;
            errorStartTime = millis();
            break;
          }
          game_play++;
          if (game_play - 1 == sequenceIncrement[level - 1]){
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
      if (!levelLost){
        if (level == levelsInGame){
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("   Juego   ");
          lcd.setCursor(0, 1);
          lcd.print("  Completado  ");
          delay(1000);
          level = 1;
          etapa = 1;  // Actualizar la variable global etapa
          gameWon = true;
        } 
        else {
            level++;
            etapa = 1;  // Actualizar la variable global etapa
        }
      }
      break;

    case 5:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("    PERDISTE   ");
      tone(buzzerPin, 350);
      for (int i = 0; i < numMusicalButtons; i++){
        digitalWrite(musicalButtons[i], HIGH); //Enciende todos los LEDs
      }
      delay(1000);
      noTone(buzzerPin);
      delay(1000);
      for (int i = 0; i < numMusicalButtons; i++){
        digitalWrite(musicalButtons[i], LOW); //Apaga todos los LEDs
      }

      etapa = 0;  // Actualizar la variable global etapa
      levelLost = false;
      lcd.clear();
      break;

    default:
      break;
  }
}

void showSequence(){
  for (int i = 1; i <= level; i++){
    pinMode(led_simonSaid[i], OUTPUT);
    digitalWrite(led_simonSaid[i], HIGH);  // Enciende el LED
    playBuzzer(led_simonSaid[i] - 1);  // Reproduce el sonido
    delay(400);
    digitalWrite(led_simonSaid[i], LOW);  // Apaga el LED
    pinMode(led_simonSaid[i], INPUT);
    delay(400);
  }
  delay(500);
}

void playBuzzer(int x){
  tone(buzzerPin, 650 + (x * 100));
  delay(300);
  noTone(buzzerPin);
}

void setButtonPinsAsInput() {
  for (int i = 0; i < numMusicalButtons; i++) {
    pinMode(musicalButtons[i], INPUT);
  }
}

void resetSequence(){
  currentPos = 0;
  lastButtonPressTime = millis();
  for (int i = 0; i < seqLen; i++){
    inputSequence[i] = -1; //llena con -1
  }
  //sequenceStarted = false;
  //Serial.println("Sequence reset."); //debug

}
               
               
void wrongSolution(){
  unsigned long currentTime = millis();
  static unsigned long lastBlinkTime = 0;  
  static bool ledState = false;
  if (currentTime - lastBlinkTime >= 500){
    lastBlinkTime = currentTime;  // Actualiza el tiempo en que parpadeo
    ledState = !ledState;
    // Enciende o apaga segun el estado del LED
    for (int i = 0; i < numLEDsInRow; i++){
      if (ledState){
        LEDsRow.setPixelColor(i, LEDsRow.Color(255, 0, 0)); // Color rojo
      } 
      else {
        LEDsRow.setPixelColor(i, LEDsRow.Color(0, 0, 0));   // Apagado
      }
    }
  
    LEDsRow.show();  // Update the strip with the new LED states
    
  }
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

void sendError(){
  unsigned long currentTime = millis();
  if (errorCommited){
    if (currentTime - errorStartTime <= timeForMistakeSignal){
      digitalWrite(badAnswerPin, HIGH);
    }
    else{
      digitalWrite(badAnswerPin, LOW);
      errorCommited = false;
    }
  }
}

void checkCilinders(){
    for (int i=0; i < numButtons; i++){
      if (digitalRead(pinCilinderButtons[i]) == HIGH){
        inputSequence[currentPos] = i+1; //contador de botones para la secuencia no empieza en 0
        lastButtonPressTime = millis();
        currentPos++;
        //sequenceStarted = true;
        //printSequence(currentPos); //debug
        while(digitalRead(pinCilinderButtons[i]) == HIGH){
          delay(100);
          //Serial.println("Waiting for release...");//debug
        }
        
        if (currentPos == seqLen){ //ya se presionaron los botones la cantidad de la secuencia
          if (isCorrectSequence()){
            //Serial.println("Correct sequence entered!");//debug
            //digitalWrite(gameSolved, HIGH);
            completedCilinders = true;
          } 
          else {
            //Serial.println("Incorrect sequence. Try again."); //debug
            errorCommited = true;
            errorStartTime = millis();
            //wrongSolution();
          }
          resetSequence();
      }
    }
  }
}

void checkTimeOut(){
  unsigned long currentTime = millis();
  if (currentPos > 0 && (currentTime - lastButtonPressTime > timeOutTime)){
    //Serial.println("Timeout! Sequence reset."); //debug
    resetSequence();
    errorCommited = true;
    errorStartTime = millis();
    //wrongSolution();
  }
}

void chasingLightPattern(){
  //if (currentPos == 0){ //si no ha comenzado la secuencia
    static int currentLED = 0; // indice del LED encendido
    static bool forward = true; // Direccion de la persecusion
    static unsigned long lastUpdateTime = 0;
    static int colorIndex = 0; //Color actual
    unsigned long currentTime = millis();
  
    //Arreglo de colores en los que ciclar
    uint32_t colors[] ={
      LEDsRow.Color(255, 0, 0),   // Rojo
      LEDsRow.Color(0, 255, 0),   // Verde
      LEDsRow.Color(0, 0, 255),   // Azul
      LEDsRow.Color(255, 255, 0), // Amarillo
      LEDsRow.Color(0, 255, 255), // Cyan
      LEDsRow.Color(255, 0, 255), // Magenta
      LEDsRow.Color(255, 255, 255) // Blanco
    };
    
    int numColors = sizeof(colors) / sizeof(colors[0]);  // Numero de colores en el arreglo
  
    // Chasing pattern: Se actualiza cada 150 milisegundos
    if (currentTime - lastUpdateTime >= 150){
      lastUpdateTime = currentTime;  // Update the last update time
  
      // Apaga todos los LEDs
      for (int i = 0; i < numLEDsInRow; i++){
        LEDsRow.setPixelColor(i, LEDsRow.Color(0, 0, 0));
      }
      // Enciende el LED actual del color del indice
      LEDsRow.setPixelColor(currentLED, colors[colorIndex]); 
  
      LEDsRow.show();
  
      // Siguiente LED
      if (forward){
        currentLED++;
        if (currentLED >= numLEDsInRow){
          currentLED = numLEDsInRow - 2; // Empieza a devolverse
          forward = false; // Cambio de direccion
          colorIndex = (colorIndex + 1) % numColors;
        }
      } 
      else { //Si iba devolviendose
        currentLED--;
        if (currentLED < 0){
          currentLED = 1; // Empieza a devolverse
          forward = true;
          colorIndex = (colorIndex + 1) % numColors;
        }
      }
    } 
  //}
}

void endGamePattern(){
  unsigned long currentTime = millis();

  // Patron de arcoiris en cilindros
  if (currentTime - lastPatternUpdate >= patternUpdateInterval){
    lastPatternUpdate = currentTime;

    // Arcoiris a cada LED
    for (int i = 0; i < numLEDsInRow; i++){
      int hue = (i * 65536L / numLEDsInRow) + rainbowIndex;  // Hue en HSV
      LEDsRow.setPixelColor(i, LEDsRow.ColorHSV(hue, 255, 255));  // Set the color
    }

    LEDsRow.show();  // Update the NeoPixel strip with new colors
    rainbowIndex += 256;  // Increment the hue index for the rainbow effect
    if (rainbowIndex >= 65536) rainbowIndex = 0;  // Wrap around the hue index
  }

  // Parpadeo en los botones musicales
  if (currentTime - lastBlinkTime >= blinkInterval){
    lastBlinkTime = currentTime;  // Update the last blink time
    buttonLEDState = !buttonLEDState;  // Toggle the state of the button LEDs

    // Parpadeo en sincronia
    for (int i = 0; i < numMusicalButtons; i++){
      digitalWrite(musicalButtons[i], buttonLEDState ? HIGH : LOW);
    }
  }
}

void blackOut(){
    if (!blackOutDone){
      for (int i = 0; i < numLEDsInRow; i++){ //Apagar LEDs cilindros
        LEDsRow.setPixelColor(i, 0);
      }
      for (int i = 0; i < numMusicalButtons; i++){ //Apagar LEDs Musicales
        digitalWrite(musicalButtons[i], LOW);
      }
      
      lcd.clear();
      lcd.noBacklight(); // apagar la lcd
      LEDsRow.show();
      cornerLED.setPixelColor(0,cornerLED.Color(0,0,0)); //apagado
      cornerLED.show();
      blackOutDone = true;
      delay(5000);
  }  
}
