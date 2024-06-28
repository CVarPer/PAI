//Declaracion Variables Switches
// PIN lectura swtiches
const int pin1 = 2;  // Pin 2 para la entrada digital 1
const int pin2 = 3;  // Pin 3 para la entrada digital 2
const int pin3 = 4;  // Pin 4 para la entrada digital 3
const int pin4 = 5;  // Pin 5 para la entrada digital 4
const int pin5 = 6;  // Pin 6 para la entrada digital 5

const int enable = 7; // cerebro approval
bool completedSwitches = false; // Game won
//Condiciones iniciales LED
bool val1 = 0;
bool val2 = 0;
bool val3 = 0;
bool val4 = 0;
bool val5 = 0;
// Pines salida LED
const int out1 = 9;  // Pin 9 para la entrada digital 1
const int out2 = 10;  // Pin 10 para la entrada digital 2
const int out3 = 11;  // Pin 1 para la entrada digital 3
const int out4 = 12;  // Pin 12 para la entrada digital 4
const int out5 = 13;  // Pin 13 para la entrada digital 5
// Solucion
const bool b1=1;//Resp1
const bool b2=0;//Resp2
const bool b3=0;//Resp3
const bool b4=1;//Resp4
const bool b5=0;//Resp5
// Estado guardado de los switches
bool est1 = 0;
bool est2 = 0;
bool est3 = 0;
bool est4 = 0;
bool est5 = 0;
// Fin Variables Switches
//Declaracion Variables PyF
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
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
byte rowPins[KPROWS] = {5,4,3,2}; //connect to the row pinouts of the keypad
byte colPins[KPCOLS] = {9,8,7,6}; //connect to the column pinouts of the keypad
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
//Comunicacion con Cerebro
int badAnswerPin = 10; //pin para enviar señal de solucion incorrecta (no usado)
//int cerebroApproval = 11; //pin para activar el modulo
int gameSolved = 12; //pin para enviar cuando se solucione correctamente

//Fin Variables PyF


void setup() {
  Serial.begin(9600);  // Inicializar la comunicación serial
  pinMode(pin1, INPUT);  // Configurar el pin como entrada
  pinMode(pin2, INPUT);
  pinMode(pin3, INPUT);
  pinMode(pin4, INPUT);
  pinMode(pin5, INPUT);
  pinMode(enable, INPUT);
  pinMode(out1, OUTPUT);  
  pinMode(out2, OUTPUT);
  pinMode(out3, OUTPUT);
  pinMode(out4, OUTPUT);
  pinMode(out5, OUTPUT);
  //pinMode(completedSwitches, OUTPUT);
  //PINMODE LEDS PyF
  lcd.init();
  lcd.clear();         
  lcd.backlight();      // backlight is on
  lcd.home();
  randomSeed(analogRead(0));
  generarNumero();
  pinMode(badAnswerPin, OUTPUT);
  //pinMode(cerebroApproval, INPUT);
  pinMode(gameSolved, OUTPUT);
  lcd.print("Hola"); //debug
  delay(2000); //debug
  lcd.clear();  
}

void loop() {
    // put your main code here, to run repeatedly:
    bool estado_en = digitalRead(enable);
    if (estado_en == HIGH && completedSwitches==false){
      bool estado_pin1 = digitalRead(pin1);
      bool estado_pin2 = digitalRead(pin2);
      bool estado_pin3 = digitalRead(pin3);
      bool estado_pin4 = digitalRead(pin4);
      bool estado_pin5 = digitalRead(pin5);
      digitalWrite(out1, estado_pin1^val1);
      est1=estado_pin1^val1;
      digitalWrite(out2, estado_pin2^val2);
      est2=estado_pin2^val2;
      digitalWrite(out3, estado_pin3^val3);
      est3=estado_pin3^val3;
      digitalWrite(out4, estado_pin2^estado_pin3^estado_pin4^val4);
      est4=estado_pin2^estado_pin3^estado_pin4^val4;  
      digitalWrite(out5, estado_pin4^estado_pin5^val5);
      est5=estado_pin4^estado_pin5^val5;
      if ((est1==b1)&&(est2==b2)&&(est3==b3)&&(est4==b4)&&(est5==b5)){
        completedSwitches=true;
     
      }
     else{
      completedSwitches=false;
    }
  }
  // else {} rutina de juego finalizado
  if (completedSwitches == true){ 
    if (gameWon==false){
      char key = teclado.getKey();

       if (key){
        Serial.print("Key "); //debug
        Serial.print(key); //debug
        Serial.println(" is pressed"); //debug
        keyPressed(key);
      }

    }
    else { // rutina finalizacion del juego
      lcd.clear();
      lcd.print("GAME WON");
      digitalWrite(gameSolved,HIGH);
      delay(500);
    }
  }
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
for (int i = 0; i < numDigitos; i++) {
        if (vectorPyF[i] != 'f') { // Si no es una fija
            for (int j = 0; j < numDigitos; j++) {
                if (CurrentTry[i] == numGenerado[j] && !encontrado[j] && j != i) { // Si es una pica
                    vectorPyF[i] = 'p'; // Marcar pica
                    encontrado[j] = true;
                    contPicas++;
                    break;
                }
            }
        }
    }

  
  numIntento++;

  endTry(vectorPyF,contFijas, contPicas);
  /*delay(1000);
  lcd.clear(); 
  lcd.setCursor(5,0); 
  lcd.print(contPicas);
  lcd.print(" picas");
  lcd.setCursor(5,1);
  lcd.print(contFijas);
  lcd.print(" fijas");*/
  
  
  
  
}

void endTry(char* picasFijas, int numFijas, int numPicas) {
    gameWon = true;
  if (numIntento == 1 && (numPicas >= numDigitos-1 || numFijas >= numDigitos-1)){
    generarNumero();
    
    checkCurrentTry(vectorDigitos, numGenerado);
    

  }
  else{
    resetVector(vectorDigitos);
    for (int i = 0; i < numDigitos; i++) {
        if (picasFijas[i] != 'f') {
            gameWon = false; // Si hay alguna pica, el jugador no ha ganado
            break;
        }
    }
    delay(1000);
    lcd.clear(); 
    // borrar al implementar los LEDs
    lcd.setCursor(5,0); //debug
    lcd.print(numPicas); //debug
    lcd.print(" picas"); //debug
    lcd.setCursor(5,1); //debug
    lcd.print(numFijas); //debug
    lcd.print(" fijas"); //debug
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
  Serial.print("Codigo a adivinar: "); //debug
  for (int i = 0; i < numDigitos; i++) { //debug
    Serial.print(numGenerado[i]); //debug
  } //debug
  Serial.println(); //debug
}
