#include <Adafruit_NeoPixel.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
//Declaracion Variables Switches
#define numSwitches  5
#define switchesLEDPin  7 //Pin donde se conecta la tira de LEDs
const int switchesPin[numSwitches] = {2,3,4,5,6}; //Pines a los que se conectan los switches
bool previousSwitchStates[numSwitches] = {0,0,0,0,0}; //Estados anteriores de los switch
const int switchesAnswer[numSwitches] = {1,0,0,1,0}; //Respuesta vista desde los LEDs
int LEDStates[numSwitches] = {};
bool completedSwitches = false;
//Comunicacion con el cerebro
#define gameSolved 8
#define badAnswerPin 9
#define cerebroApproval 10
#define roadStart 11

Adafruit_NeoPixel switchesLED = Adafruit_NeoPixel(numSwitches, switchesLEDPin, NEO_RGB + NEO_KHZ800);
// Fin Variables Switches
//Declaracion Variables PyF

LiquidCrystal_I2C lcd(0x27, 16, 2); //I2C address and screen size

const byte KPROWS = 4; //four rows
const byte KPCOLS = 4; //four columns
char keys[KPROWS][KPCOLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// Conexion de los pines derecha a izquierda pin 2,3,4,5,6,7,8,9
//byte rowPins[KPROWS] = {5,4,3,2}; //connect to the row pinouts of the keypad
//byte colPins[KPCOLS] = {9,8,7,6}; //connect to the column pinouts of the keypad
byte rowPins[KPROWS] = {15,14,13,12}; //connect to the row pinouts of the keypad
byte colPins[KPCOLS] = {19,18,17,16}; //connect to the column pinouts of the keypad
Keypad teclado = Keypad( makeKeymap(keys), rowPins, colPins, KPROWS, KPCOLS );

int cursorCol=2; //0 a 15
int cursorRow=0; //0 y 1

const int numDigitos=5;
char vectorDigitos[numDigitos+1];
int longitud = 0;
bool numRepetido = false;
bool gameWon = false;
int numIntento=0;
char numGenerado[numDigitos];
#define PyF_LEDPin 20

Adafruit_NeoPixel PyF_LED = Adafruit_NeoPixel(numDigitos, PyF_LEDPin, NEO_RGB + NEO_KHZ800);
//Fin Variables PyF

bool blackOutDone = false;
unsigned long previousMillisSwitches = 0;  //  tiempo switchesLED
unsigned long previousMillisPyF = 0;       // tiempo PyF_LED
int waitSwitches = 500;                    // Intervalo de tiempo entre parpadeo milliseconds
int waitPyF = 20;                          // Intervalo de tiempo entre cambio de color milliseconds
long firstPixelHueSwitches = 0; //Transicion de colores HSV
long firstPixelHuePyF = 0;


void setup() {
  Serial.begin(9600);  // Inicializar la comunicación serial
  for (int i = 0; i < numSwitches; i++){
    pinMode(switchesPin[i], INPUT);
  }
  //pinMode(completedSwitches, OUTPUT);
  //PINMODE LEDS PyF
  lcd.init();
  lcd.clear();         
  lcd.backlight();      // backlight is on
  lcd.home();
  randomSeed(analogRead(0));
  generarNumero();
    pinMode(badAnswerPin, OUTPUT);
  pinMode(cerebroApproval, INPUT);
  pinMode(gameSolved, OUTPUT);
  pinMode(roadStart, OUTPUT);
  digitalWrite(roadStart, HIGH); //es inicio de camino
  digitalWrite(badAnswerPin, LOW); //inicializar
  digitalWrite(gameSolved, LOW);
  switchesLED.begin();
  switchesLED.show();
  PyF_LED.begin();
  PyF_LED.show();

}

void loop(){
  if (digitalRead(cerebroApproval) == HIGH){
    if (!completedSwitches){
      bool switchesStates[numSwitches]; //estado actual de los switches
      for (int i = 0; i < numSwitches; i++){ //Leer el estado de todos los switches
        switchesStates[i] = digitalRead(switchesPin[i]);
      }
      updateLEDStates(switchesStates);
      if (checkLEDAnswer()){
        completedSwitches = true;
      }
    }
    else{ //Switches completado, inicio de Picas y Fijas
      switchesLED.setPixelColor(numSwitches, switchesLED.Color(0, 255, 0)); //verde
      switchesLED.show();
      if (gameWon == false){
       char key = teclado.getKey();
       if (key){
          //Serial.print("Key "); //debug
          //Serial.print(key); //debug
          //Serial.println(" is pressed"); //debug
          keyPressed(key);
        }
      }
      else { // rutina finalizacion del juego
        //lcd.clear();
        lcd.print("GAME COMPLETED");
        digitalWrite(gameSolved, HIGH);
        updateWavePattern();   //Patron de luz para PyF
        updateBlinkingPattern(); //Patron de Luz para Switches
        
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

void updateLEDStates(bool currentSwitchStates[]){
  for (int i = 0; i < numSwitches; i++){
    // Revisa un cambio de estado en un switch
    if (currentSwitchStates[i] != previousSwitchStates[i]){
      // Si son diferentes
      toggleLEDsAffectedBySwitch(i); //llama la funcion cada que ve uno diferente
      previousSwitchStates[i] = currentSwitchStates[i];
    }
  }

  switchesLED.show();  // Update the LED strip with new colors
}

void toggleLEDsAffectedBySwitch(int switchIndex) { 
  // La forma en que afecta cada switch sobre los LED
  switch (switchIndex){
    case 0:{ //Switch 1 afecta todos los LEDs
      for (int i = 0; i < numSwitches; i++){
        LEDStates[i] = !LEDStates[i];
      }
    }
    case 1:{ //Switch 2 afecta LED 2
      LEDStates[2] = !LEDStates[2];
    }
    case 2:{ //Switch 3 afecta LED 1, 3, 5
      LEDStates[1] = !LEDStates[1];
      LEDStates[3] = !LEDStates[3];
      LEDStates[5] = !LEDStates[5];
    }
    case 3:{ //Switch 4 afecta LED 2, 5
      LEDStates[2] = !LEDStates[2];
      LEDStates[5] = !LEDStates[5];
    }
    case 4:{ //Switch 5 afecta LED 3, 4
      LEDStates[3] = !LEDStates[3];
      LEDStates[4] = !LEDStates[4];
    }
  }

  // Actualiza los LEDs con los nuevos estados
  for (int i = 0; i < numSwitches; i++) {
    setLEDState(i, LEDStates[i]);
  }
}

void setLEDState(int index, bool state){
  if (state){
    switchesLED.setPixelColor(index, switchesLED.Color(0, 255, 0));  // Verde for ON
  } 
  else {
    switchesLED.setPixelColor(index, switchesLED.Color(255, 0, 0));  // Rojo for OFF
  }
}

bool checkLEDAnswer(){
  for (int i = 0; i < numSwitches; i++){
    if (LEDStates[i] != switchesAnswer[i]){
      return false;  // Devuelve falso si algun LED esta distinto
    }
  }
  return true;  // Solucionado correctamente
}

void keyPressed( char key){
    if (key=='*'){ // borrar el ultimo caracter
      if (longitud >0){
        longitud--;
        vectorDigitos[longitud]='\0';
        lcd.clear();
        cursorCol=2;
        lcd.setCursor(cursorCol,cursorRow);
        lcd.print(vectorDigitos);
        cursorCol+=longitud;

      }
      
    }
    else if (key=='#'){ // aceptar la combinacion ingresada
      if (longitud == numDigitos){
        lcd.clear();
        cursorCol=2;
        cursorRow=0;
        //lcd.setCursor(cursorCol,cursorRow);
        lcd.print("Codigo Aceptado");
        checkCurrentTry(vectorDigitos, numGenerado);
      }
        
    }
    else{ // tecla no especial presionada
      if (key != '\0' && longitud < numDigitos){
        numRepetido = false;
        for (int i=0; i<longitud; i++){
          if (key==vectorDigitos[i]){
            numRepetido=true;
          }
        }
        if (numRepetido==false){
          vectorDigitos[longitud]=key;
          lcd.setCursor(cursorCol,cursorRow);
          //lcd.print(key);
          lcd.clear();
          cursorCol=2;
          lcd.setCursor(cursorCol,cursorRow);
          lcd.print(vectorDigitos);
          cursorCol++;
          longitud++;
          vectorDigitos[longitud]='\0';        
        }
      }
  }
}

void checkCurrentTry(char* CurrentTry, char* numGenerado){ // 'p' si obtuvo una pica, 'f' si obtuvo una fija
  char vectorPyF[numDigitos]={};
  bool encontrado[numDigitos] ={false}; //no vuelve a revisar si ya fue encontrado
  int contPicas=0, contFijas=0;
  for (int i=0; i< numDigitos; i++){
    if (CurrentTry[i]==numGenerado[i]){
      vectorPyF[i]='f'; // Marcar Fija
      encontrado[i]=true;
      contFijas++;
     }
   }
  for (int i = 0; i < numDigitos; i++){
        if (vectorPyF[i] != 'f') { // Si no es una fija
            for (int j = 0; j < numDigitos; j++){
                if (CurrentTry[i] == numGenerado[j] && !encontrado[j] && j != i){ // Si es una pica
                    vectorPyF[i] = 'p'; // Marcar pica
                    encontrado[j] = true;
                    contPicas++;
                    break;
                }
            }
        }
  }
  endTry(vectorPyF,contFijas, contPicas); 
}

void endTry(char* picasFijas, int numFijas, int numPicas){
    gameWon = true;
  if (numIntento == 0 && (numPicas >= numDigitos-1 || numFijas >= numDigitos-1 || (numPicas+numFijas)>= numDigitos-1)){ //Proteccion contra buena suerte
    generarNumero();
    checkCurrentTry(vectorDigitos, numGenerado);
  }
  else{
    numIntento++;
    resetVector(vectorDigitos);
    set_pyf_LEDColors(numFijas, numPicas);
    
    for (int i = 0; i < numDigitos; i++){
        if (picasFijas[i] != 'f'){
            gameWon = false; // Si hay alguna pica, el jugador no ha ganado
            break;
        }
    }
    delay(1000);
    lcd.clear(); 
    // borrar al implementar los LEDs
    //lcd.setCursor(5,0); //debug
    //lcd.print(numPicas); //debug
    //lcd.print(" picas"); //debug
    //lcd.setCursor(5,1); //debug
    //lcd.print(numFijas); //debug
    //lcd.print(" fijas"); //debug
  }
    //return gameWon;
}

void resetVector(char* vectorDigitos){
  for (int i = 0; i < numDigitos; ++i) {
        vectorDigitos[i] = '\0'; 
  }
  cursorCol=2;
  cursorRow=0;
  longitud=0;
}

void generarNumero(){
  char opcionesChar[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 'A', 'B', 'C', 'D'};
  int cantidadChar=sizeof(opcionesChar);
  int size=0;
  bool repetido=false;
  
  while(size<numDigitos){
    int index = random(cantidadChar);
    char randomChar = opcionesChar[index];
    //Serial.print("Caracter aleatorio: ");
    //Serial.println(randomChar);
    for (int i=0; i<size;i++){
      if (randomChar == numGenerado[i]){
        repetido=true;
      }
    }
    if(repetido==false){
        numGenerado[size]=randomChar;
        size++;
        //Serial.print("Agregado: ");
      //Serial.println(randomChar);
    }
    repetido=false;
  }
  //Serial.print("Codigo a adivinar: "); //debug
  //for (int i = 0; i < numDigitos; i++) { //debug
    //Serial.print(numGenerado[i]); //debug
  //} //debug
  //Serial.println(); //debug
}

void set_pyf_LEDColors(int fijas, int picas){
  // LEDs para fijas (posicion y digito correcto)
  for (int i = 0; i < fijas; i++){
    PyF_LED.setPixelColor(i, PyF_LED.Color(0, 255, 0));  // Verde
  }

  // LEDs para picas (posicion incorrecta, digito correcto)
  for (int i = fijas; i < picas + fijas; i++){
    PyF_LED.setPixelColor(i, PyF_LED.Color(255, 255, 0));  // amarillo
  }

  // LEDs restantes (posicion y digito incorrecto)
  for (int i = fijas + picas; i < numDigitos; i++){
    PyF_LED.setPixelColor(i, PyF_LED.Color(255, 0, 0));  // Rojo
  }
  PyF_LED.show();
}

void blackOut(){
    if (!blackOutDone){
      for (int i = 0; i < numSwitches; i++){ //Apagar LEDs Switches
        switchesLED.setPixelColor(i, 0);
      }
      for (int i = 0; i < numDigitos; i++){ //Apagar LEDs PyF
        PyF_LED.setPixelColor(i, 0);
      }
      
      lcd.clear();
      lcd.noBacklight(); // apagar la lcd
      blackOutDone = true;
      delay(5000);
  }  
}

void updateWavePattern(){
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillisPyF >= waitPyF){
    previousMillisPyF = currentMillis;  

    // Efecto de ola
    for (int i = 0; i < numDigitos; i++){
      int pixelHue = firstPixelHuePyF + (i * 65536L / numDigitos);  // Calcular HUE para cada LED
      PyF_LED.setPixelColor(i, PyF_LED.ColorHSV(pixelHue));  // Asignar color con el HUE
    }

    PyF_LED.show();
    firstPixelHuePyF += 256;  // Aumento del HUE para el siguiente ciclo
    if (firstPixelHuePyF >= 65536){
      firstPixelHuePyF = 0;  // Reset hue
    }
  }
}


void updateBlinkingPattern(){
  unsigned long currentMillis = millis(); 
  static bool isOn = false;  // Saber si los LEDs estan encendidos o apagados

  if (currentMillis - previousMillisSwitches >= waitSwitches){
    previousMillisSwitches = currentMillis;
    if (isOn){
      // Apagar todos los LEDs
      for (int i = 0; i < numSwitches; i++){
        switchesLED.setPixelColor(i, 0);
      }
      isOn = false;
    } 
    else {
      uint32_t color = switchesLED.ColorHSV(firstPixelHueSwitches); //Calcula el color para el parpadeo

      for (int i = 0; i < numSwitches; i++){
        switchesLED.setPixelColor(i, color);
      }
      firstPixelHueSwitches += 256;  
      if (firstPixelHueSwitches >= 65536){
        firstPixelHueSwitches = 0; 
      }
      isOn = true;
    }
    switchesLED.show();  
  }
}
