#include <Arduino.h>
#include <LiquidCrystal.h>
#include <ArduinoSTL.h>
#include <vector>

// defining variables related to button
const int buttonPin = 12; // pin for button input
int count; // counter for how many times the button is pressed
bool isPressed; // boolean value for whether or not the button is pressed
unsigned long lastPressTime = 0; // to track the time of the last button press
const unsigned long debounceDelay = 50; // debounce time in milliseconds
const unsigned long displayDelay = 1000; // time to wait after last press before displaying count

// defining variables for segment LED module pin connections
const int a = 2;
const int b = 3;
const int c = 4;
const int d = 5;
const int e = 6;
const int f = 7;
const int g = 8;
const int dp = 9;

// defining a variable to hold counting numbers of presses
int presses = 0;

void displayZero() { // sets display on LED to 0
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, HIGH);
  digitalWrite(f, HIGH);
  digitalWrite(g, LOW);
  digitalWrite(dp, LOW);
}

void displayNumber(int number) { // function that shows a number to display on LED module
  switch (number) {
    case 1:
      digitalWrite(b, HIGH);
      digitalWrite(c, HIGH);
      break;

    case 2:
      digitalWrite(a, HIGH);
      digitalWrite(b, HIGH);
      digitalWrite(g, HIGH);
      digitalWrite(e, HIGH);
      digitalWrite(d, HIGH);
      break;

    case 3:
      digitalWrite(a, HIGH);
      digitalWrite(b, HIGH);
      digitalWrite(g, HIGH);
      digitalWrite(c, HIGH);
      digitalWrite(d, HIGH);
      break;

    case 4:
      digitalWrite(f, HIGH);
      digitalWrite(g, HIGH);
      digitalWrite(b, HIGH);
      digitalWrite(c, HIGH);
      break;

    case 5:
      digitalWrite(a, HIGH);
      digitalWrite(f, HIGH);
      digitalWrite(g, HIGH);
      digitalWrite(c, HIGH);
      digitalWrite(d, HIGH);
      break;

    case 6:
      digitalWrite(a, HIGH);
      digitalWrite(f, HIGH);
      digitalWrite(g, HIGH);
      digitalWrite(e, HIGH);
      digitalWrite(c, HIGH);
      digitalWrite(d, HIGH);
      break;

    case 7:
      digitalWrite(a, HIGH);
      digitalWrite(b, HIGH);
      digitalWrite(c, HIGH);
      break;

    case 8:
      digitalWrite(a, HIGH);
      digitalWrite(f, HIGH);
      digitalWrite(g, HIGH);
      digitalWrite(e, HIGH);
      digitalWrite(c, HIGH);
      digitalWrite(d, HIGH);
      digitalWrite(b, HIGH);
      break;

    case 9:
      digitalWrite(a, HIGH);
      digitalWrite(f, HIGH);
      digitalWrite(g, HIGH);
      digitalWrite(c, HIGH);
      digitalWrite(d, HIGH);
      digitalWrite(b, HIGH);
      break;
    
    default:
      displayZero(); // For 0 or invalid number, show 0
      break;
  }
}

void clearDisplay() { // clears the LED display
  digitalWrite(a, LOW);
  digitalWrite(b, LOW);
  digitalWrite(c, LOW);
  digitalWrite(d, LOW);
  digitalWrite(e, LOW);
  digitalWrite(f, LOW);
  digitalWrite(g, LOW);
  digitalWrite(dp, LOW);
}

void blinking() { // blinks the dp on LED to show it is ready to process counts
  digitalWrite(dp, HIGH);
  delay(500);
  digitalWrite(dp, LOW);
  delay(500);
}

void counting() {
  if (isPressed) {
    count++;
    delay(200);  // debounce delay to avoid multiple counts for a single press
    while (digitalRead(buttonPin) == HIGH) {
      // nothing happens to wait until the button is released
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  int pin; // initializing the pin variable for board
  for (pin = a; pin <= dp; pin++) { // loops through each digital pin and sets it to output
    pinMode(pin, OUTPUT);
  }

  pinMode(buttonPin, INPUT); // sets button as an input on board

  // shows initializing at startup -> if not seen, something is wrong
  displayZero();
  delay(100);
  clearDisplay();
  delay(100);
  displayZero();
  delay(100);
  clearDisplay();
  delay(100);
  displayZero();
  delay(100);
  clearDisplay();
  delay(100);
}

void loop() {
  unsigned long currentTime = millis(); // Get the current time

  if (digitalRead(buttonPin) == HIGH) { // Check if the button is pressed
    if (!isPressed && (currentTime - lastPressTime > debounceDelay)) { // If it wasn't already pressed and debounce time has passed
      isPressed = true; // Mark it as pressed
      count++; // Increment the count
      lastPressTime = currentTime; // Update the time of the last press
    }
  } else {
    isPressed = false; // Mark as not pressed when the button is released
  }

  // Check if enough time has passed since the last press to display the count
  if (count > 0 && (currentTime - lastPressTime > displayDelay)) {
    displayNumber(count); // Display the count
    delay(2000); // Display the number for 2 seconds
    clearDisplay(); // Clear the display
    delay(500); // Small delay before resetting
    count = 0; // Reset the count
  }
}
