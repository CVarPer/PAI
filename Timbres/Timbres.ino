#include <Adafruit_NeoPixel.h>
#include <math.h>

const int numButtons = 4;
const int pinButtons [numButtons] = {2,3,4,5};

int seqLen = 0; // Cada boton se presiona 1 vez
//orden en que se deben presionar los botones numerados del 1 al 4
const int numSeq = 4;
int Sequence[numSeq] = {1,2,3,4};
int timesPerButton[numSeq] = {1,2,3,4}; //numero de veces que se presiona cada boton relacionado a Sequence[numSeq]
int* correctSequence = nullptr;
int* inputSequence = nullptr;
// tiempo maximo permitido para presionar el numero de veces de la secuencia
const int timeOutTime = 5000; //entre botones
const int timeOutClick = 3000; //entre veces que se debe presionar
int currentPos = 0;
int timesPressed = 0;
unsigned long lastButtonPressTime = 0;
unsigned long lastClickTime = 0;

#define cornerLEDPin 6
Adafruit_NeoPixel cornerLED = Adafruit_NeoPixel(1, cornerLEDPin, NEO_RGB + NEO_KHZ800);

//Joystick
#define numRingLEDs 7
#define ringLEDPin 7
#define JS_X A0 // Posicion X del Joystick
#define JS_Y A1 // Posicion Y del Joystick
#define JS_button 12 // Pulsador del Joystick
Adafruit_NeoPixel ringLED = Adafruit_NeoPixel(numRingLEDs, ringLEDPin, NEO_RGB + NEO_KHZ800);

uint32_t colors[] = {
  ringLED.Color(255, 0, 0),   // Red
  ringLED.Color(0, 255, 0),   // Green
  ringLED.Color(0, 0, 255),   // Blue
  ringLED.Color(255, 255, 0), // Yellow
  ringLED.Color(0, 255, 255), // Cyan
  ringLED.Color(255, 0, 255), // Magenta
  ringLED.Color(255, 255, 255), // White
  0 //Apagado
};
int numColors = sizeof(colors) / sizeof(colors[0]);
float minMoveThreshold = 300.0; // Minimo que se debe mover el joystick para ser reconocido
int currentColorIndex = 0; // Color actual
int lastButtonState = LOW; // Estado Inicial del boton


//Comunicacion con el cerebro
#define gameSolved 8
#define badAnswerPin 9
#define cerebroApproval 10
#define roadStart 11
bool gameWon = false;

unsigned long timeForMistakeSignal = 3000; //Tiempo durante el que se envia la señal de error
unsigned long errorStartTime = 0;
bool errorCommited = false;

bool blackOutDone = false;

void setup(){
  Serial.begin(9600);
  for (int i=0; i < numButtons; i++){
     pinMode(pinButtons[i], INPUT);
  }
  pinMode(badAnswerPin, OUTPUT);
  pinMode(cerebroApproval, INPUT);
  pinMode(gameSolved, OUTPUT);
  
  for (int i = 0; i < numSeq; i++){ //total de veces que hay presionar todos los botones
        seqLen += timesPerButton[i];
  }
  correctSequence = new int[seqLen];
  inputSequence = new int[seqLen];
  int index = 0;
  for (int i = 0; i < numSeq; i++){
      for (int j = 0; j < timesPerButton[i]; j++){
        //Por cada boton, guardar el numero de veces en el vector
          correctSequence[index] = Sequence[i];
          inputSequence[index] = 0;
          index++;
      }
  }
  pinMode(JS_button, INPUT);
  ringLED.begin();
  ringLED.show();
  //Serial.print("index: "); //debug
  //Serial.println(index); //debug
  //Serial.println(seqLen); //debug
  int* inputSequence = new int[seqLen];
  //resetSequence();
  //printSequence(seqLen); //debug
  pinMode(badAnswerPin, OUTPUT);
  pinMode(cerebroApproval, INPUT);
  pinMode(gameSolved, OUTPUT);
  pinMode(roadStart, OUTPUT);
  digitalWrite(roadStart, LOW); //NO es inicio de camino
  digitalWrite(badAnswerPin, LOW); //inicializar
  digitalWrite(gameSolved, LOW);
}


void loop(){
  if (digitalRead(cerebroApproval) == HIGH){
    cornerLED.setPixelColor(0,cornerLED.Color(0,255,0)); //verde
    cornerLED.show();
   if (!gameWon){
      checkButtons();
      checkTimeOut();
      selectLED();

      if (errorCommited){
        //wrongSolution();
        sendError();    
      }
    }
    else{ //fin de juego
      digitalWrite(gameSolved, HIGH);
      cornerLED.setPixelColor(0,cornerLED.Color(127,0,255)); //morado
      cornerLED.show();
      brushPattern();
    }
  }
  else {
    if (gameWon){
      pinMode(roadStart, INPUT);
      if (digitalRead(roadStart) == HIGH){
        blackOut();
      }
    }
  }
}

void checkButtons(){
    for (int i=0; i < numButtons; i++){
      if (digitalRead(pinButtons[i]) == HIGH){
        inputSequence[currentPos] = i+1; //contador de botones para la secuencia no empieza en 0
        currentPos++;
        while(digitalRead(pinButtons[i]) == HIGH){
          delay(100);
          //currentTime = millis();
          //Serial.println("Waiting for release...");//debug
        }
        lastButtonPressTime = millis();
        if (currentPos == seqLen){ //ya se presionaron los botones la cantidad de la secuencia
          if (isCorrectSequence()){
            //Serial.println("Correct sequence entered!");//debug
            digitalWrite(gameSolved, HIGH);
            gameWon = true;
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

void resetSequence(){
  currentPos = 0;
  lastButtonPressTime = millis();
  //Serial.println(seqLen); //debug
  for (int i = 0; i < seqLen; i++) {
    inputSequence[i] = -1; //llena con -1
    //Serial.println(i);//debug
  }
  //Serial.println("Sequence reset."); //debug
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

void checkTimeOut(){
  unsigned long currentTime = millis();
  bool timedOut = false;
  if (currentPos > 0){ //Si ya inicio a ingresar la secuencia
    if (correctSequence[currentPos] == correctSequence[currentPos+1]){ //Si esta presionando repetidamente el mismo boton
      if (currentTime - lastButtonPressTime > timeOutClick){ //Menor tiempo entre clicks que entre la secuencia
        timedOut = true;
      }
    }
    else{ //Sigue un boton distinto
      if (currentTime - lastButtonPressTime > timeOutTime){
        timedOut = true;
      }
    }
  }
  if (timedOut){
    resetSequence();
    errorCommited = true;
    errorStartTime = millis();
  }
}

void blackOut(){
    if (!blackOutDone){
      for (int i = 0; i < numRingLEDs; i++){ //Apagar LEDs joystick
        ringLED.setPixelColor(i, 0);
      }
      
      ringLED.show();
      cornerLED.setPixelColor(0,cornerLED.Color(0,0,0)); //apagado
      cornerLED.show();
      blackOutDone = true;
      delay(5000);
  }  
}

void selectLED(){
  static int lastLEDIndex = -1; //evitar estar llamando a la funcion de mostrar
  static int selectedLED = -1; // LED seleccionado
  // Lectura de la posicion X y Y del Joystick
  int xValue = analogRead(JS_X) - 512; //rango de -512 a 512
  int yValue = analogRead(JS_Y) - 512;

  //Calculo de la distancia respecto al centro
  float distance = sqrt((float)xValue * (float)xValue + (float)yValue * (float)yValue);
  // Si el movimiento del joystick fue mayor al umbral
  if (distance > minMoveThreshold) {
    // Calculo del angulo
    float angle = atan2((float)yValue, (float)xValue);
    if (angle < 0) {
      angle += 2 * PI;
    }

    // Map el angulo entre (0 a numRingLEDs-1)
    int ledIndex = (int)(angle / (2 * PI) * numRingLEDs);

    if (ledIndex != lastLEDIndex){ //si se selecciono un led diferente
      selectedLED = ledIndex; // Update the selected LED to the current LED index
      lastLEDIndex = ledIndex; // Save the last LED index to avoid unnecessary updates
    }
  }
  int buttonState = digitalRead(JS_button);
  if (buttonState == HIGH && lastButtonState == LOW){
    // Si se presiono el boton
    if (selectedLED != -1){ // Asegurar que se selecciono un LED
      currentColorIndex = (currentColorIndex + 1) % numColors;
      ringLED.setPixelColor(selectedLED, colors[currentColorIndex]);
      ringLED.show();
    }
  }
  
  lastButtonState = buttonState;
}

void brushPattern(){
  static int lastLEDIndex = -1; // Ultimo indice del LED encontrado

  // Lectura de la posicion X y Y del Joystick
  int xValue = analogRead(JS_X) -512; //Se centra el valor respecto a 0 
  int yValue = analogRead(JS_Y) -512; //rango de -512 a 512
  //Calculo de la distancia respecto al centro
  float distance = sqrt((float)xValue * (float)xValue + (float)yValue * (float)yValue);
  
 // Si el movimiento del joystick fue mayor al umbral
  if (distance > minMoveThreshold){
    // Calcular el angulo y mapearlo entre 0 y 2PI
    float angle = atan2((float)yValue, (float)xValue);
    if (angle < 0){
      angle += 2 * PI; // Angulos negativos a angulos positivos
    }
    int ledIndex = (int)(angle / (2 * PI) * numRingLEDs);

    // Si la posicion del Joystick cambio
    if (ledIndex != lastLEDIndex){
      // Encender el LED encontrado con el color actual
      if (ringLED.getPixelColor(ledIndex) == 0){
        ringLED.setPixelColor(ledIndex, colors[currentColorIndex]);
      } 
      else {
        ringLED.setPixelColor(ledIndex, 0); // Apagar
      }

      ringLED.show();
      lastLEDIndex = ledIndex;
    }
  }

  // Pulsador presionado
  int buttonState = digitalRead(JS_button);
  if (buttonState == HIGH && lastButtonState == LOW){
    // Si se presiono
    currentColorIndex = (currentColorIndex + 1) % numColors;
  }
  
  lastButtonState = buttonState;
}
