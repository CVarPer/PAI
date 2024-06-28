
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
const int gameSolved = 12; //pin para enviar cuando se solucione correctamente
bool gameWon=false;

void setup(){
  Serial.begin(9600);
  for (int i=0; i < numButtons; i++){
     pinMode(pinFirstButton + i, INPUT);
   }
  pinMode(pinLed, OUTPUT);
  pinMode(badAnswerPin, OUTPUT);
  pinMode(cerebroApproval, INPUT);
  pinMode(gameSolved, OUTPUT);
}


void loop() {
  //if (digitalRead(cerebroApproval) == HIGH){
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
            digitalWrite(gameSolved, HIGH);
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
