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

const int numDigitos=6;
char vectorDigitos[numDigitos];
int longitud = 0;
bool numRepetido = false;
bool gameWon = false;
int numIntento=0;
char numGenerado[numDigitos]={'A','1','C','3','9'};


void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.clear();         
  lcd.backlight();      // Make sure backlight is on
   
}

void loop(){
  if (gameWon==false){
    char key = teclado.getKey();
     
     if (key){
      Serial.print("Key ");
      Serial.print(key);
      Serial.println(" is pressed");
      keyPressed(key);
    }
    
  }
  else {
    lcd.clear();
    lcd.print("GAME WON");
    delay(500);
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
      if (longitud == numDigitos-1){
        lcd.clear();
        cursorCol=2;
        cursorRow=0;
        //lcd.setCursor(cursorCol,cursorRow);
        lcd.print("Codigo Aceptado");
        checkCurrentTry(vectorDigitos, numGenerado);
      }
        
    }
    else{ // tecla no especial presionada
      if (key != '\0' && longitud < numDigitos-1){
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
  for (int i=0; i< numDigitos-1; i++){
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
  //Serial.print(numIntento);
  
  delay(1000);
  lcd.clear();
  /*lcd.setCursor(5,0);
  lcd.print(contPicas);
  lcd.print(" picas");
  lcd.setCursor(5,1);
  lcd.print(contFijas);
  lcd.print(" fijas");*/
  
  lcd.setCursor(1,0);
  lcd.print("Your last try: ");
  lcd.setCursor(5,1);
  lcd.print(vectorDigitos)
  resetVector(vectorDigitos);
  endTry(vectorPyF);
  
  
}

void endTry(char* picasFijas) {
    gameWon = true; 
    for (int i = 0; i < numDigitos-1; i++) {
        if (picasFijas[i] != 'f') {
            gameWon = false; // Si hay alguna pica, el jugador no ha ganado
            break;
        }
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
