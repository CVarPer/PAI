//
// Arduino Mega tiene D0 - D13 (D0 y D1) son TX0 y RX0 (Pines PWM)
// D14-D21 son TX3/RX3 - TX2/RX2 - TX1/RX1 - SDA/SCL
// D22 - D53 meramente digitales
// D54 - D69 (A0 - A15)

#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define penitenceLedPin 10   // pin del LED de penitencia
#define stripLedPin 12     // pin de la tira LED

#define NUMPIXELS 20 // numero de LEDs en la tira

Adafruit_NeoPixel penitence = Adafruit_NeoPixel(1, penitenceLedPin, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, stripLedPin, NEO_GRB + NEO_KHZ800);

const int numModules = 3;
int correctAnswerPin[numModules] = {2,6,14};
//int correctAnswerPin[numModules] = {22,26,30,34,38};
int wrongAttemptPin[numModules] = {3,7,15};
//int wrongAttemptPin[numModules] = {23,27,31,35,39};
int cerebroApproval[numModules] = {4,8,16};
//int cerebroApproval[numModules] = {24,28,32,36,40};
int roadStarter[numModules] = {5,9,17};
//int roadStarter[numModules] = {25,29,33,37,41};

// variables para el LED de penitencia
int redColor = 0;
int greenColor = 0;
int blueColor = 0;
int blinkingTime= 5000;
unsigned long patternStartTime = 0;
unsigned long previousPatternTime = 0;
int delayTime = 300;
bool patternActive = false;
bool ledState = false;
int currentPenitentModule = numModules+1;

//variables para el temporizador
typedef struct{
  int hours;
  int minutes;
  int seconds;
} temporizador;
temporizador timer = {0, 30, 15}; //1h 30 min 0 s de tiempo inicial
const int initialMinutes = timer.minutes + timer.hours*60;
const int initialSeconds = initialMinutes*60;
long interval = 1000; //Intervalo de tiempo para disminuir el temporizador
unsigned long previousTimerTime = 0;
int timeFactor = 1; //varaible para cambiar la velocidad con la que pasa el tiempo

//variables para el buzzer
#define buzzerPin 11
int sound = 0;
bool playingSound = false;
unsigned long previousSoundTime = 0;
unsigned long mistakeStartTime = 0;
int mistakeStep = 0; //paso de reproduccion del sonido de error
bool soundPlayed = false; //flag para saber si ya se reprodujo todo el sonido de penitencia


//Inicio del juego
#define startButtonPin  13

bool starterGun = false; //da comienzo al timer
unsigned long previousStripTime = 0;
int stripInterval = 150;
int currentLED = 0;

//Variables para patrones cinta LED
int brightness = 0; //0 - 255
int fadeAmount = 5; //Cambio de brillo por ciclo
long fadeInterval = 150;
long blinkInterval = 100;
unsigned long fadeTime = 0;
bool redPattern = false; //Banderas para reasignar valores a las variables unica vez
bool orangePattern = false;
bool yellowPattern = false;
bool fadeOut = false; //Fade in o fade out
int hue=0; // 0 - 65535 in HSV
unsigned long colorTime = 0; //Control del tiempo para cambio de color
int colorInterval = 100; //tiempo entre cambios de color

//Activacion de modulos
bool activatedModules[numModules] = {false};
#define timeForNextActivation 1200000 //tiempo en ms, para activar el siguiente modulo
bool firstModuleActivated = false;
unsigned long lastActivationTime = 0; //trackeo del tiempo de la ultima activacion

//Fin del juego
#define redButtonPin 18
#define toggleSwitchPin 19 //switches del modulo del boton rojo
bool gameLost = false;
bool gameWon = false;
unsigned long messageStartTime = 0; //Tiempo de inicio del mensaje LCD
unsigned long displayDuration = 5000; //Tiempo de display del mensaje LCD
bool displayingMessage = false;
const char* defaultMsg = "Desactiva la bomba";
bool blackoutDone = false;


LiquidCrystal_I2C lcd(0x27, 16, 2);



  
void setup(){
  for (int i=0; i<numModules; i++){
    pinMode(correctAnswerPin[i], INPUT);
    pinMode(wrongAttemptPin[i], INPUT);
    pinMode(cerebroApproval[i], OUTPUT);
    pinMode(roadStarter[i], INPUT);
  }
  pinMode(buzzerPin, OUTPUT);
  pinMode(startButtonPin, INPUT);
  lcd.init();
  lcd.clear();         
  lcd.backlight();      // backlight is on
  lcd.home();
  penitence.begin();
  penitence.show();
  strip.begin();
  strip.show();
  Serial.begin(9600);
  randomSeed(analogRead(0));
}

void loop(){
  unsigned long currentMillis = millis();
  if (starterGun == false){ //logica que sucede antes de comenzar el juego
    if (digitalRead(startButtonPin)){
      starterGun = true;
      //reiniciar displays
      lcd.clear();
      setStripColor(1,0,0,0);
    }
    else{ //Antes de que se presione el boton
      if (currentMillis - previousStripTime >= stripInterval){  
        movingLightPattern();
        //displayShowZeros();
        pressButtonToStartLCD();
      }
    }
  }
  else{ //una vez presionado el boton
    if (!gameLost && !gameWon){ //tiempo de juego
      checkPenitence(); //revisar si se cumple la condicion de penitencia
      modulesActivation();
      timedActivation();
      //staticTimedActivation();
      checkRedButton();
      stripGamePattern();
      countDownSound();
      if (currentMillis - previousTimerTime >= interval){
        previousTimerTime = currentMillis;
        decreaseTime();
        printLCD();
        //printMessageLCD(defaultMsg);
        //showTimer();
      }
    }
    else if (gameLost){ //juego perdido
      
      // EXPLOTAR GLOBO SENAL SERVO
      //blackout();
      //endGameDisplay();
    }
    else {//juego ganado
      //blackout();
      //endGameDisplay();
      //endGameStrip();
      
    }
  }
}



void checkPenitence(){
  if (patternActive == false){
    for (int i=0; i<numModules; i++){
      if(digitalRead(wrongAttemptPin[i]) == HIGH){
        patternActive = true;
        patternStartTime = millis();
        currentPenitentModule = i;
        setPenitenceColor();
        soundPlayed = false;
      }
    }
    
   }
  else {
    for (int i=0; i<numModules; i++){ 
      //no permitir que sigan jugando
      digitalWrite(cerebroApproval[i], LOW);
    }
    blinkPenitencePattern();
    if (!soundPlayed){
      wrongAnswerSound();
    }
  }

}

void setPenitenceColor(){
    int penitenceNumber = random(0,100); 
    if (penitenceNumber < 30){ //30% chance
      penitenceNumber = 0;
    }
    else if (penitenceNumber < 40){ //10% chance
      penitenceNumber = 1;
    }
    else if (penitenceNumber < 55){ //15% chance
      penitenceNumber = 2;
    }
    else if (penitenceNumber < 65){ //10% chance
      penitenceNumber = 3;
    }
    else if (penitenceNumber < 85){ //20% chance
      penitenceNumber = 4;
    }
    else { //15% chance
      penitenceNumber = 5;
    }


    //escoge aleatoriamente entre las 5 posibles penitencias, cada penitencia esta indicada por un color de LED
    // Penitencia 1: Perder tiempo
    // Penitencia 2: Perder un miembro por 5 minutos
    // Penitencia 3: 5 minutos sin poder hablar
    // Penitencia 4:
    // Penitencia 5:


    switch(penitenceNumber){
      case 0:{ //Penitencia 1, color rojo
          redColor = 255;
          greenColor = 0;
          blueColor = 0;
          //disminuir tiempo del temporizador
          int lostTime = 5;
          decreaseMinutes(lostTime);
          break;}
      case 1:{ //Penitencia 2, color verde
          redColor = 0;
          greenColor = 255;
          blueColor = 0;
          break;}
      case 2:{ //Penitencia 3, color azul
          redColor = 0;
          greenColor = 0;
          blueColor = 255;
          break;}
      case 3:{ //Penitencia 4, color magenta
          redColor = 255;
          greenColor = 0;
          blueColor = 255;
          break;}
      case 4:{ //Penitencia 5, color cyan
          redColor = 0;
          greenColor = 255;
          blueColor = 255;
          break;}
      case 5:{ //Penitencia 6, color amarillo
          redColor = 255;
          greenColor = 255;
          blueColor = 0;
          break;}
    }
  blinkPenitencePattern();
}

void blinkPenitencePattern(){
    unsigned long currentPatternTime = millis();
    unsigned long elapsedTime = currentPatternTime - patternStartTime; //tiempo transcurrido en el parpadeo

    if (elapsedTime < 2000){
      // Primeros 2 segundos - patron estatico
      setStripColor(0, redColor, greenColor, blueColor);
    } 
    else if (elapsedTime < blinkingTime){
      // Ultimos 3 segundos - parpadeo
      if (currentPatternTime - previousPatternTime >= delayTime){
        previousPatternTime = currentPatternTime;

        if (ledState){ //si esta prendido apagar
          setStripColor(0, 0, 0, 0); // Turn off
        } 
        else {
          setStripColor(0, redColor, greenColor, blueColor); // Turn on
        }

        ledState = !ledState;

        // Aumentar la frecuencia del parpadeo, hasta un minimo de 50 ms
        delayTime = max(50, delayTime - 10);
      }
    } 
  else {
    // Despues del tiempo, apaga Penitencia
    setStripColor(0, 0, 0, 0);
    patternActive = false;  // Fin del patron
    currentPenitentModule = numModules+1;
    resetActivatedModules(); 
    //vuelve a encender los modulos que estaban activos
  }
}

void setStripColor(int identifier, int r, int g, int b){
  if (identifier == 0){
    penitence.setPixelColor(0, penitence.Color(r, g, b));
    penitence.show();
  }
  else {
    for (int i = 0; i < NUMPIXELS; i++){ //los prende todos del mismo color al mismo tiempo
      strip.setPixelColor(i, strip.Color(r, g, b));
    }
    strip.show();
  }
}

void decreaseTime(){
  // Decrementar 1 segundo
  if (timer.seconds > 0){
    timer.seconds--;
  }
  else {
    timer.seconds = 59;
    if (timer.minutes > 0){
      timer.minutes--;
    }
    else {
      timer.minutes = 59;
      if (timer.hours > 0){
        timer.hours--;
      }
      else {
        // Tiempo llega a 0
        timer.hours = 0;
        timer.minutes = 0;
        timer.seconds = 0;
        gameLost = true;
        
      }
    }
  }
}

void decreaseMinutes(int minutesToDecrease) {
  int minutesLeft = timer.hours * 60 + timer.minutes;

  // Confirmar que hay suficiente tiempo restante
  if (minutesLeft >= minutesToDecrease){
    minutesLeft -= minutesToDecrease;  // Quitar el tiempo

    // Actualizar el temporizador
    timer.hours = minutesLeft / 60;
    timer.minutes = minutesLeft % 60;
  } 
  else {
    // Si se acaba el tiempo
    timer.hours = 0;
    timer.minutes = 0;
    timer.seconds = 0;
  }
}

void countDownSound(){
  unsigned long currentTime = millis();
  
  int stage = 1;  // Etapa del juego segun el tiempo restante
  
  
  int pitch, duration, soundInterval;
  int secondsLeft = timer.seconds + timer.minutes*60 + timer.hours*3600;
  
  if (secondsLeft <= 300){ //menos de 5 min
    stage=3;
  }
  else if (secondsLeft <= 900){ // menos de 15 min
    stage = 2;
  }

  if (stage == 1){ //Desde el inicio hasta 15 minutos restantes
    pitch = 698;
    duration = 50;
    soundInterval = 1000;
  } 
  else if (stage == 2){ //Desde 15 minutos hasta 5 minutos
    pitch = 698;
    duration = 50;
    soundInterval = 750;  // cada 0.75 s
  } 
  else if (stage == 3){
    pitch = 78;
    duration = 200;
    soundInterval = 500; // cada 0.5 s
  }

  if (currentTime - previousSoundTime >= soundInterval){
    //reproducir sonido pasado el tiempo
    previousSoundTime = currentTime;
    if (!playingSound){
      tone(buzzerPin, pitch, duration);
    }
  }
}
  

void printLCD(){
  // Formato hh:mm:ss
  char buffer[9];
  snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", timer.hours, timer.minutes, timer.seconds);

  
  lcd.setCursor(0, 0);
  lcd.print("Tiempo Restante:");
  lcd.setCursor(0, 1);
  lcd.print(buffer);         // Muestra el tiempo en formato hh:mm:ss
}

void wrongAnswerSound(){
  unsigned long currentTime = millis();
  if (!playingSound){
    playingSound = true;
    mistakeStartTime = currentTime;
    mistakeStep = 0;
    noTone(buzzerPin);
  }
  //Secuencia de tonos del sonido de error
  
  switch (mistakeStep){
  case 0:
      tone(buzzerPin, 1000, 150);  // Primer tono
      mistakeStep++;
      previousSoundTime = currentTime;
      break;
    case 1:
      if (currentTime - previousSoundTime >= 200){
        tone(buzzerPin, 800, 150);  // Segundo tono
        mistakeStep++;
        previousSoundTime = currentTime;
      }
      break;
    case 2:
      if (currentTime - previousSoundTime >= 200){
        tone(buzzerPin, 600, 150);  // Tercer tono
        mistakeStep++;
        previousSoundTime = currentTime;
      }
      break;
    case 3:
      if (currentTime - previousSoundTime >= 200){
        tone(buzzerPin, 400, 150);  // Cuarto tono
        mistakeStep++;
        previousSoundTime = currentTime;
      }
      break;
    case 4: //reiniciar las variables
      if (currentTime - previousSoundTime >= 200) {
        noTone(buzzerPin);
        playingSound = false;
        soundPlayed = true;
      }
      break;
  }

}

void movingLightPattern(){
  unsigned long currentStripTime = millis();
  
  if (currentStripTime - previousStripTime >= stripInterval) {
    previousStripTime = currentStripTime;
  int previousLED = (currentLED - 1 + NUMPIXELS) % NUMPIXELS;
    // Apagar los 2 anteriores
    strip.setPixelColor(currentLED, strip.Color(0, 0, 0));
    strip.setPixelColor(previousLED, strip.Color(0, 0, 0));

    // Moverse a los 2 siguientes, vuelve al inicio
    currentLED = (currentLED + 1) % NUMPIXELS;
    previousLED++;

    //Encender los 2 siguientes
    uint32_t randomColor = strip.Color(random(0, 256), random(0, 256), random(0, 256));
    strip.setPixelColor(currentLED, randomColor);
    strip.setPixelColor(previousLED, randomColor);
    
    
    strip.show();
  }
}

void pressButtonToStartLCD(){
  lcd.setCursor(1, 0);
  lcd.print("Presiona para");
  lcd.setCursor(4, 1);
  lcd.print("Comenzar"); 
}

void stripGamePattern(){
  unsigned long currentTime = millis();
  int secondsLeft = timer.seconds + timer.minutes*60 + timer.hours*3600;
  if(secondsLeft <=300){ //Si quedan menos de 5 minutos
    // Color rojo, disminucion progresiva del brillo
    if (redPattern == false){
      redPattern = true;
      brightness = 255;
      fadeAmount = -5;
      fadeInterval = 30;
      blinkInterval = 100;
    }
    
    if(currentTime - fadeTime >= fadeInterval){
      fadeTime = currentTime;
      brightness = max(0, brightness + fadeAmount);
      
    int r = 255, g = 0, b = 0;
      scaleColorBrightness(r, g, b, brightness);
      setStripColor(1, r, g, b);
      if (brightness == 0){
        brightness = 255;
      }
    }

  }
  else if (secondsLeft <=900){ //Si quedan menos de 15 minutos
    // Color naranja, Fade in lento, Fade out rapido 
    if (orangePattern == false){
      orangePattern = true;
      brightness = 0;
      fadeAmount = 5;
      fadeInterval = 50;
      blinkInterval = 150;
      fadeOut = false;
    }
    if (secondsLeft <=360){ //patron parpadeo el ultimo minuto
      if (currentTime - fadeTime >= blinkInterval){
        fadeTime = currentTime;
        brightness = (brightness == 0) ? 255 : 0;  // Alterna entre 0 y 255
        int r = 255, g = 69, b = 0;
        scaleColorBrightness(r, g, b, brightness);
        setStripColor(1, r, g, b);
      }
    }
    else{
      if (currentTime - fadeTime >= fadeInterval){
        fadeTime = currentTime;
        if (fadeOut == false){ //fade in
          brightness += fadeAmount;
        }
        else{ //fade out
          brightness -= 2*fadeAmount;
        }
    brightness = constrain(brightness, 0, 255);
        // Cambio de direccion
        
        if (brightness >= 255){
          fadeOut = true;
        }
        else if (brightness <= 0){
          fadeOut = false;
        }

        int r = 255, g = 69, b = 0;  
        scaleColorBrightness(r, g, b, brightness);
        setStripColor(1, r, g, b);  
      }
    }
  }
  else if (secondsLeft > (initialMinutes/2)*60){ //Si queda mas de la mitad de los minutos iniciales
    //Arcoiris
    // Tiempo entre cambio de brillo
      if (currentTime - fadeTime >= fadeInterval){
        fadeTime = currentTime;
        brightness += fadeAmount;
        if (brightness >= 255 || brightness <= 0){
          fadeAmount = -fadeAmount;
        }
      }

    // Tiempo entre cambio de color
      if (currentTime - colorTime >= colorInterval){
        colorTime = currentTime;

        hue += 256;  // Cambio del hue
        if (hue >= 65536) hue = 0;  // Reinicia despues del ciclo
      }

    // Color segun brillo y hue
    for (int i = 0; i < NUMPIXELS; i++){
      uint32_t color = strip.ColorHSV(hue, 255, brightness);  // Convert HSV to RGB, use full saturation
      strip.setPixelColor(i, color);
    }

    strip.show();
  }
  else{ //Tiempo mayor a 15 min y menor a la mitad de tiempo
    // Color Amarillo, luz parpadeante
    if (yellowPattern == false){
      yellowPattern = true;
      brightness = 255;
      fadeAmount = -5;
      fadeInterval = 50;
      blinkInterval = 100;
    }
    if (currentTime - fadeTime >= fadeInterval) {
      fadeTime = currentTime;

      // Brillo random para efecto de parpadeo luz danada
      fadeAmount = random(-80, 80);  
      brightness = constrain(brightness + fadeAmount, 60, 255);  //valores entre 100 y 255

      int r = 255, g = 255, b = 0;  // Amarillo
      scaleColorBrightness(r, g, b, brightness);
      setStripColor(1, r, g, b);
    }
    
  }
}

void scaleColorBrightness(int &r, int &g, int &b, int bright){ 
  //escala el color segun el brillo
  r = (r*bright)/255;
  g = (g*bright)/255;
  b = (b*bright)/255;
}

void modulesActivation(){
  
  int numStarter = 0;
  int starterIndex[numModules];
  
  if (!firstModuleActivated){
    for (int i=0; i<numModules; i++){
      if (digitalRead(roadStarter[i]) == HIGH){
        starterIndex[numStarter] = i;
        numStarter++;
      }
    }
    
    if (numStarter >0){
      //Selecciona aleatoriamente el modulo inicial
      int first = random(0, numStarter);
      // int first = 0; //si se desea el primer modulo o alguno en especifico
      int index = starterIndex[first];
      digitalWrite(cerebroApproval[index], HIGH);
      firstModuleActivated = true;
      activatedModules[index] = true;
      lastActivationTime = millis();
    }
  }
  
  for (int i = 0; i < numModules; i++){
    if (digitalRead(correctAnswerPin[i]) == HIGH && patternActive == false){ 
      //modulo solucionado y no esta sucediendo penitencia
      if (digitalRead(roadStarter[i]) == HIGH){ //inicio de camino
        bool nonStarterCompleted = true;
        for (int j = 0; j < numModules; j++){
          if (digitalRead(roadStarter[j]) == LOW){ 
            //encender todos lo no inicio de camino
            digitalWrite(cerebroApproval[j], HIGH);
            activatedModules[j] = true;
            if (digitalRead(correctAnswerPin[j]) == HIGH){ //Si no inicio ya esta completo
              nonStarterCompleted = true && nonStarterCompleted;
            }
            else {
              nonStarterCompleted = false;
            }
          }
        }
        if (nonStarterCompleted){ 
          //si todos los no iniciales fueron completados y el ultimo completado fue un inicio
          for (int j = 0; j < numModules; j++){
              if (digitalRead(roadStarter[j]) == HIGH && digitalRead(correctAnswerPin[j]) == LOW){
                digitalWrite(cerebroApproval[j], HIGH);
                activatedModules[j] = true;
                lastActivationTime = millis();
                break;
              }
            }
        }
      }
      else { //si el completado no es inicio de camino
        for (int j = 0; j < numModules; j++){
          if (digitalRead(roadStarter[j]) == HIGH && digitalRead(correctAnswerPin[j]) == LOW){
            digitalWrite(cerebroApproval[j], HIGH);
            activatedModules[j] = true;
            lastActivationTime = millis();
            break;
          }
        }
      } 
    }
  }
}
  
void resetActivatedModules(){
  // Vuelve a encender todos los que estaban activos antes de penitencia
  for (int i=0; i<numModules; i++){
    if (activatedModules[i]){
      digitalWrite(cerebroApproval[i], HIGH);
    }
  }
}

void timedActivation(){ 
  //Activacion pasado 20 minutos desde la ultima activacion
  unsigned long currentTime = millis();
  if (currentTime - lastActivationTime >= timeForNextActivation){
    for (int i=0; i<numModules; i++){
      if (digitalRead(roadStarter[i]) == HIGH && !activatedModules[i]){
        digitalWrite(cerebroApproval[i], HIGH);
        activatedModules[i] = true;
        lastActivationTime = currentTime;
        break;  // Activa solo 1 modulo
      }
    }
  }
}

void staticTimedActivation(){
  //activacion pasado 20 minutos del inicio del juego
  unsigned long currentTime = millis();
  int secondsLeft = timer.hours*3600 + timer.minutes*60 + timer.seconds;
  if (initialSeconds - secondsLeft >= timeForNextActivation/1000){
    for (int i=0; i<numModules; i++){
      if (digitalRead(roadStarter[i]) == HIGH && !activatedModules[i]){
        digitalWrite(cerebroApproval[i], HIGH);
        activatedModules[i] = true;
        lastActivationTime = currentTime;
        break;  // Activa solo 1 modulo
      }
    }
  }
}

void checkRedButton(){
  bool modulesCompleted = false;  
  bool switchActivated = false;   
  
  if (digitalRead(redButtonPin) == HIGH){ //si se presiono el boton
    // No han completado los modulos
    if (!modulesCompleted){
      // Penalizacion por presionarlo antes de tiempo
      printMessageLCD("Boton presionado antes de tiempo");
      int timeLost = 5;
      decreaseMinutes(timeLost);
    }
    
    // Si completaron los modulos y no activaron los switches
    else if (modulesCompleted && !switchActivated){
      // Penalizacion por presionarlo antes de tiempo
      printMessageLCD("Boton presionado antes de tiempo");
      int timeLost = 5;
      decreaseMinutes(timeLost);
    }
    // Correct sequence: Modulos completados luego switches
    else if (modulesCompleted && switchActivated){
      // Fin del juego
      gameWon = true;
    }
  }
  // Verificar si se completaron todos los modulos
  if (areAllModulesCompleted()){
    modulesCompleted = true;
  }
  // Solo se pueden activar los switches una vez completaron los modulos
  if (modulesCompleted && digitalRead(toggleSwitchPin) == HIGH) {
    switchActivated = true;  
  }
}

bool areAllModulesCompleted() {
  for (int i = 0; i < numModules; i++) {
    if (digitalRead(correctAnswerPin[i]) == LOW) {
      return false;  
    }
  }
  return true;  
}

void printMessageLCD(const char* message){
  unsigned long currentTime = millis();
  

  if (strcmp(message, defaultMsg) != 0) { //0 si son iguales 1 si son diferentes
    // Si se ingreso un mensaje y no se esta mostrando mensaje
    if (!displayingMessage) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(message);  
      displayingMessage = true;
      messageStartTime = currentTime;  // Tiempo de inicio del mensaje
    }
  } 
  else { //la funcion se llamo con el mensaje por defecto
    // Mensaje por defecto una vez terminado el tiempo del mensaje
    if (displayingMessage && (currentTime - messageStartTime >= displayDuration)){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(defaultMsg);  
      displayingMessage = false;
    }
    
    // Asegurarse de poner el mensaje default
    if (!displayingMessage){
      lcd.setCursor(0, 0);
      lcd.print(defaultMsg);  
    }
  }
}

void blackout(){ //apagar todo
  if (!blackoutDone){
    // poner los pines de inicio de carrera en output
    for (int i=0; i<numModules; i++){
      pinMode(roadStarter[i], OUTPUT);
      digitalWrite(roadStarter[i], HIGH); // enviar 1 en los pines
    }
    setStripColor(0, 0, 0, 0);
    setStripColor(1, 0, 0, 0); // apagar tira
    noTone(buzzerPin);
    lcd.clear();
    lcd.noBacklight(); // apagar la lcd
    
    
    // apagar display
    
    blackoutDone = true;
    delay(5000);
  }
}

void endGameDisplay(){
  if (gameWon){
    
  }
  else {
    
  }
}
void endGameStrip(){
  

}
