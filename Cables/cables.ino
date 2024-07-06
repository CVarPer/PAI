const int cable1 = 2; 
const int cable2 = 3;  
const int cable3 = 4;  
const int cable4 = 5;  
const int cable5 = 6;  
const int cable6 = 7;  
const int cable7 = 8;
const int error = 9;
const int salida = 10;
bool est1 = 0;
bool est2 = 0;
bool est3 = 0;
bool est4 = 0;
bool est5 = 0;
bool est6 = 0;
bool est7 = 0;
void setup() {
  // Configurar cada pin como entrada
  pinMode(cable1, INPUT);
  pinMode(cable2, INPUT);
  pinMode(cable3, INPUT);
  pinMode(cable4, INPUT);
  pinMode(cable5, INPUT);
  pinMode(cable6, INPUT);
  pinMode(cable7, INPUT);  
  pinMode(error, OUTPUT);
  pinMode(salida, OUTPUT);
}

void loop() {
  bool e1 = digitalRead(cable1);
  bool e2 = digitalRead(cable2);
  bool e3 = digitalRead(cable3);
  bool e4 = digitalRead(cable4);
  bool e5 = digitalRead(cable5);
  bool e6 = digitalRead(cable6);
  bool e7 = digitalRead(cable7);
  if ((est1 == LOW)&&(e1!= HIGH)){
  digitalWrite(error, HIGH);
  est1=1;
  delay(3000);
  digitalWrite(error, LOW);  
  }
  if ((est2 == LOW)&&(e2!= HIGH)){
  digitalWrite(error, HIGH);
  est2=1;
  delay(3000);
  digitalWrite(error, LOW);  
  }
  if ((est4 == LOW)&&(e4!= HIGH)){
  digitalWrite(error, HIGH);
  est4=1;
  delay(3000);
  digitalWrite(error, LOW);  
  }
  if ((est5 == LOW)&&(e5!= HIGH)){
  digitalWrite(error, HIGH);
  est5=1;
  delay(3000);
  digitalWrite(error, LOW);  
  }
  if ((est6 == LOW)&&(e6!= HIGH)){
  digitalWrite(error, HIGH);
  est6=1;
  delay(3000);
  digitalWrite(error, LOW);  
  }
  if ((est7 == LOW)&&(e7!= HIGH)){
  digitalWrite(error, HIGH);
  est7=1;
  delay(3000);
  digitalWrite(error, LOW);  
  }
  if ((est3 == LOW)&&(e3!= HIGH)){
  digitalWrite(salida, HIGH); 
  est1=1;
  est2=1;
  est3=1;
  est4=1;
  est5=1;
  est6=1;
  est7=1;
  }
}
