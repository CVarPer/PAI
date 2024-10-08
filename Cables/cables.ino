
#define numCables 7
const int cablePins[numCables] = {2, 3, 4, 5, 6, 7, 8};
const int solenoidPin = 13;
bool disconnectedCables[numCables] = {false}; //Variable para almacenar los cables que ya fueron desconectados para no volver a penalizar
#define gameSolved 10
#define badAnswerPin 11
#define cerebroApproval 9
#define roadStart 12

const int correctCable = 3; //Pin al que se conecta el cable correcto que se debe cortar
unsigned long timeForMistakeSignal = 4000; //Tiempo durante el que se envia la señal de error
unsigned long errorStartTime = 0;
bool errorCommited = false;
bool gameWon = false;

unsigned long timeToDeactivation = 15; //Tiempo en minutos desde que se activo el solenoide
unsigned long timeFromSolenoidActivation = 0;

void setup(){
  for (int i = 0; i < numCables; i++){
    pinMode(cablePins[i], INPUT);
  }
  pinMode(solenoidPin, OUTPUT);
  pinMode(badAnswerPin, OUTPUT);
  pinMode(cerebroApproval, INPUT);
  pinMode(gameSolved, OUTPUT);
  pinMode(roadStart, OUTPUT);
  digitalWrite(roadStart, HIGH); //es inicio de camino
  digitalWrite(badAnswerPin, LOW); //inicializar
  digitalWrite(gameSolved, LOW);
  digitalWrite(solenoidPin, LOW);
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
      if (cablePins[i] != correctCable){
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
      errorCommited = false;
    }
  }
}
void activateSolenoid(){
  digitalWrite(solenoidPin, HIGH);
  timeFromSolenoidActivation = millis();
}
