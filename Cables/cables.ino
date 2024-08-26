
#define numCables 7
const int cablePins[numCables] = {2, 3, 4, 5, 6, 7, 8};
const int solenoidPin = 9;
bool disconnectedCables[numCables] = {}; //Variable para almacenar los cables que ya fueron desconectados para no volver a penalizar
#define gameSolved 8
#define badAnswerPin 9
#define cerebroApproval 10
#define roadStart 11

const int correctCable = 2; //Pin al que se conecta el cable correcto que se debe cortar
unsigned long timeForMistakeSignal = 3000; //Tiempo durante el que se envia la señal de error
unsigned long errorStartTime = 0;
bool errorCommited = false;
bool gameWon = false;

unsigned long timeToDeactivation = 15; //Tiempo en minutos desde que se activo el solenoide
unsigned long timeFromSolenoidActivation = 0;

void setup(){
  for (int i = 0; i < numCables; i++){
    pinMode(cablePins[i], INPUT);
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
      checkCables();
      sendError();
    }
    else {
      //fin del juego
      unsigned long currentTime = millis();
      if (currentTime - timeFromSolenoidActivation >= (timeToDeactivation*60000)){ //min a ms
        //Si ya paso el tiempo para desactivar el solenoide, desenergizarlo para no consumir corriente
        digitalWrite(solenoidPin, LOW);
      }
    }
  }
  else {
    //blackout
  }
}

void checkCables(){
  // Los cables tienen una resistencia de Pulldown
  // Cuando estan conectados se lee HIGH
  // Si se lee LOW es porque se desconecto el cable
  for (int i = 0; i < numCables; i++){
    if (digitalRead(cablePins[i]) == LOW){
      //Se desconecto el cable
      if (i != correctCable){
        if(!disconnectedCables[i]){ //si no ha sido desconectado ya
          errorCommited = true;
          disconnectedCables[i] = true;
          errorStartTime = millis();
        }
      }
      else {
        //Se corto el cable correcto
        gameWon = true;
        digitalWrite(gameSolved, HIGH);
        activateSolenoid();
      }
    }
  }
}

void sendError(){
  unsigned long currentTime = millis();
  if (errorCommited){
    if (currentTime - errorStartTime <= timeForMistakeSignal){
      digitalWrite(badAnswerPin, HIGH);
    }
    else{
      digitalWrite(badAnswerPin, LOW);
    }
  }
}
void activateSolenoid(){
  digitalWrite(solenoidPin, HIGH);
  timeFromSolenoidActivation = millis();
}
