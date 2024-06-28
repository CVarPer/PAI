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
int cerebroApproval = 11; //pin para activar el modulo
int gameSolved = 12; //pin para enviar cuando se solucione correctamente

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.clear();         
  lcd.backlight();      // backlight is on
  lcd.home();
  randomSeed(analogRead(0));
  generarNumero();
  pinMode(badAnswerPin, OUTPUT);
  pinMode(cerebroApproval, INPUT);
  pinMode(gameSolved, OUTPUT);
  lcd.print("Hola");
  delay(2000);
  lcd.clear();
}

void loop(){
  if (digitalRead(cerebroApproval) == HIGH){ 
    if (gameWon==false){
      char key = teclado.getKey();

       if (key){
        Serial.print("Key "); //debug
        Serial.print(key); //debug
        Serial.println(" is pressed"); //debug
        keyPressed(key);
      }

    }
    else {
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

void createCharacter(){
  byte humanHead[8] = {
      0b00000,
      0b01110,
      0b10101,
      0b10101,
      0b10101,
      0b01110,
      0b00000,
      0b00000
  };
  lcd.createChar(0, humanHead);

}
