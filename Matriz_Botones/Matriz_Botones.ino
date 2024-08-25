#include <Keypad.h>

// C++ code
//
const byte ROWS = 3; //Numero de filas del teclado que se esta usando
const byte COLS = 3; //Numero de columnas del teclado que se esta usando
 
char hexaKeys[ROWS][COLS] =  //Aquí pondremos la disposición de los caracteres tal cual están en nuestro teclado
{
  {'a','b','c'},
  {'d','e','f'},
  {'g','h','i'}
};
 
byte rowPins[ROWS] = {A0, A1, A2}; //Seleccionamos los pines en el arduino donde iran conectadas las filas
byte colPins[COLS] = {A3, A4, A5}; //Seleccionamos los pines en el arduino donde iran conectadas las columnas
 
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); //inicializa el teclado

int a1 [3]= {2,3,5};
int a2 [4]= {2,3,4,6};
int a3 [3]= {3,4,7};
int b1 [4]= {2,5,6,8};
int b2 [5]= {3,5,6,7,9};
int b3 [4]= {4,6,7,10};
int c1 [3]= {5,8,9};
int c2 [4]= {6,8,9,10};
int c3 [3]= {7,9,10};

void setup() {
  // Configurar los pines 2 a 10 como salidas digitales
  for (int i = 2; i <= 10; i++) {
    pinMode(i, OUTPUT);
  }
  pinMode (A0, INPUT);
  pinMode (A1, INPUT);
  pinMode (A2, INPUT);
  pinMode (A3, INPUT);
  pinMode (A4, INPUT);
  pinMode (A5, INPUT);
  
  Serial.begin(9600);
}

void loop()
{
  char customKey = customKeypad.getKey(); //se guarda en la variable customKey el caracter de la tecla presionada
  if (customKey != NO_KEY)         //se evalúa si se presionó una tecla
  {
    Serial.println(customKey);    //se imprime en el puerto serie la tecla presionada
  	if (customKey == 'a')
    {
     for (int i = 0; i < sizeof(a1); i++) 
     {
        // Leer el estado actual del pin
        int estadoActual = digitalRead(a1[i]);
    
        // Cambiar al estado opuesto
        int nuevoEstado = (estadoActual == HIGH) ? LOW : HIGH;
    
        // Establecer el nuevo estado en el pin
        digitalWrite(a1[i], nuevoEstado);
        delay(10);
     }
	 
    }
    
    if (customKey == 'b')
    {
     for (int i = 0; i < sizeof(a2); i++)
      {
        // Leer el estado actual del pin
        int estadoActual = digitalRead(a2[i]);
    
        // Cambiar al estado opuesto
        int nuevoEstado = (estadoActual == HIGH) ? LOW : HIGH;
    
        // Establecer el nuevo estado en el pin
        digitalWrite(a2[i], nuevoEstado);
        delay(10);
      }
    }
    if (customKey == 'c')
    {
     for (int i = 0; i < sizeof(a3); i++)
     {
        // Leer el estado actual del pin
        int estadoActual = digitalRead(a3[i]);
    
        // Cambiar al estado opuesto
        int nuevoEstado = (estadoActual == HIGH) ? LOW : HIGH;
    
        // Establecer el nuevo estado en el pin
        digitalWrite(a3[i], nuevoEstado);
        delay(10);
      }
    }
    if (customKey == 'd')
    {
     for (int i = 0; i < sizeof(b1); i++) 
     {
        // Leer el estado actual del pin
        int estadoActual = digitalRead(b1[i]);
    
        // Cambiar al estado opuesto
        int nuevoEstado = (estadoActual == HIGH) ? LOW : HIGH;
    
        // Establecer el nuevo estado en el pin
        digitalWrite(b1[i], nuevoEstado);
        delay(10);
      }
    }
    if (customKey == 'e')
    {
     for (int i = 0; i < sizeof(b2); i++)
     {
        // Leer el estado actual del pin
        int estadoActual = digitalRead(b2[i]);
    
        // Cambiar al estado opuesto
        int nuevoEstado = (estadoActual == HIGH) ? LOW : HIGH;
    
        // Establecer el nuevo estado en el pin
        digitalWrite(b2[i], nuevoEstado);
        delay(10);
      }
    }
    if (customKey == 'f')
    {
     for (int i = 0; i < sizeof(b3); i++)
     {
        // Leer el estado actual del pin
        int estadoActual = digitalRead(b3[i]);
    
        // Cambiar al estado opuesto
        int nuevoEstado = (estadoActual == HIGH) ? LOW : HIGH;
    
        // Establecer el nuevo estado en el pin
        digitalWrite(b3[i], nuevoEstado);
        delay(10);
      }
    }
    if (customKey == 'g')
    {
     for (int i = 0; i < sizeof(c1); i++) 
     {
        // Leer el estado actual del pin
        int estadoActual = digitalRead(c1[i]);
    
        // Cambiar al estado opuesto
        int nuevoEstado = (estadoActual == HIGH) ? LOW : HIGH;
    
        // Establecer el nuevo estado en el pin
        digitalWrite(c1[i], nuevoEstado);
        delay(10);
      }
    }
    if (customKey == 'h')
    {
     for (int i = 0; i < sizeof(c1); i++) 
     {
        // Leer el estado actual del pin
        int estadoActual = digitalRead(c2[i]);
    
        // Cambiar al estado opuesto
        int nuevoEstado = (estadoActual == HIGH) ? LOW : HIGH;
    
        // Establecer el nuevo estado en el pin
        digitalWrite(c2[i], nuevoEstado);
        delay(10);
      }
    }
    if (customKey == 'i')
    {
     for (int i = 0; i < sizeof(c3); i++) 
     {
        // Leer el estado actual del pin
        int estadoActual = digitalRead(c3[i]);
    
        // Cambiar al estado opuesto
        int nuevoEstado = (estadoActual == HIGH) ? LOW : HIGH;
    
        // Establecer el nuevo estado en el pin
        digitalWrite(c3[i], nuevoEstado);
        delay(10);
      }
    }
   digitalWrite(LED_BUILTIN, !digitalRead(13));   
  };
}