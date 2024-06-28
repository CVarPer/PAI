// Configuración de los LEDs RGB (3 pines por LED)
#define NUM_LEDS 5
#define PIN_R {6, 7, 8, 9, 10}  // Pines para el color rojo
#define PIN_G {11, 12, 13, 14, 15}  // Pines para el color verde
#define PIN_B {16, 17, 18, 19, 20}  // Pines para el color azul

void setup() {
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(PIN_R[i], OUTPUT);
    pinMode(PIN_G[i], OUTPUT);
    pinMode(PIN_B[i], OUTPUT);
  }
}

void loop() {
  // Aquí debes obtener las picas y fijas después de cada intento
  // Supongamos que las tienes en las variables 'picas' y 'fijas'

  // Apaga todos los LEDs
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(PIN_R[i], LOW);
    digitalWrite(PIN_G[i], LOW);
    digitalWrite(PIN_B[i], LOW);
  }

  // Enciende los LEDs según las picas y fijas
  for (int i = 0; i < fijas; i++) {
    digitalWrite(PIN_G[i], HIGH);  // Verde para fijas
  }
  for (int i = fijas; i < fijas + picas; i++) {
    digitalWrite(PIN_R[i], HIGH);  // Rojo para picas
    digitalWrite(PIN_G[i], HIGH);  // Verde también para picas
  }

  // Espera un tiempo antes de continuar al siguiente intento
  delay(2000);  // Puedes ajustar este valor según tus preferencias
}
