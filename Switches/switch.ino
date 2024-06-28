const int pin1 = 2;  // Pin 2 para la entrada digital 1
const int pin2 = 3;  // Pin 3 para la entrada digital 2
const int pin3 = 4;  // Pin 4 para la entrada digital 3
const int pin4 = 5;  // Pin 5 para la entrada digital 4
const int pin5 = 6;  // Pin 6 para la entrada digital 5
const int enable = 7;
const int comp = 8;
bool val1 = 0;
bool val2 = 0;
bool val3 = 0;
bool val4 = 0;
bool val5 = 0;
const int out1 = 9;  // Pin 9 para la entrada digital 1
const int out2 = 10;  // Pin 10 para la entrada digital 2
const int out3 = 11;  // Pin 1 para la entrada digital 3
const int out4 = 12;  // Pin 12 para la entrada digital 4
const int out5 = 13;  // Pin 13 para la entrada digital 5
const bool b1=1;//Resp1
const bool b2=0;//Resp2
const bool b3=0;//Resp3
const bool b4=1;//Resp4
const bool b5=0;//Resp5
bool est1 = 0;
bool est2 = 0;
bool est3 = 0;
bool est4 = 0;
bool est5 = 0;

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
  pinMode(comp, OUTPUT);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  bool estado_en = digitalRead(enable);
  if (estado_en == HIGH){
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
  digitalWrite(comp, HIGH);
   
  }else{
  digitalWrite(comp, LOW);
  }
  }
}
