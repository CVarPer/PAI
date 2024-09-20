#include <MD_Parola.h>
#include <MD_MAX72XX.h>
#include <SPI.h>

// Define the hardware type and number of devices (8x8 LED modules)
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 5  // Adjust this to match your setup (5 matrices in a row)
#define DIN_PIN A0      // Chip Select Pin
#define CS_PIN A1      // Chip Select Pin
#define CLK_PIN A2      // Chip Select Pin


// Initialize the Parola object
MD_Parola display = MD_Parola(HARDWARE_TYPE, DIN_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
// Timer structure and initial setup
typedef struct {
  int hours;
  int minutes;
  int seconds;
} temporizador;

temporizador timer = {1, 30, 15}; // Set initial time to 0h 30m 15s

// Variables for non-blocking timer management
unsigned long previousMillis = 0;
const long interval = 1000;  // 1 second interval for countdown

bool gameLost = false;  // Flag to indicate when the timer reaches 0

// Function to decrease the time
void decreaseTime() {
  // Decrement 1 second
  if (timer.seconds > 0) {
    timer.seconds--;
  } else {
    timer.seconds = 59;
    if (timer.minutes > 0) {
      timer.minutes--;
    } else {
      timer.minutes = 59;
      if (timer.hours > 0) {
        timer.hours--;
      } else {
        // Time has reached 0
        timer.hours = 0;
        timer.minutes = 0;
        timer.seconds = 0;
        gameLost = true;  // Set flag when timer reaches zero
      }
    }
  }
}

// Function to format and display time on the LED matrix
void displayTime() {
  char timeString[9];  // hh:mm:ss format
  snprintf(timeString, sizeof(timeString), " %01d:%02d:%02d", timer.hours, timer.minutes, timer.seconds);
  display.print(timeString);  // Display the formatted time
}

void setup() {
  display.begin();     // Initialize the display
  display.setIntensity(5);  // Set display brightness (0-15)
  display.displayClear();   // Clear any previous display data
}

void loop() {
  // Check if 1 second has passed for the non-blocking timer
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval && !gameLost) {
    previousMillis = currentMillis;  // Update the time reference
    decreaseTime();  // Decrease timer by 1 second
    displayTime();   // Update the display with the new time
  }
}
