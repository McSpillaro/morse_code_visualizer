// All libraries are installed from PlatformIO libraries onto the project (not sys dependent)
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <ArduinoSTL.h>
#include <vector>
#include <iostream>
#include <string>

/*
LiquidCrystalDisplay (LCD) - Morse Code

Displays the inputted morse code letters on the LCD panel.
This sketch takes input from a button and displays proper characters
that correspond to the proper morse code.

After holding the button for 'x' amount of time, it will clear the display.

The circuit (LCD module):
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
   * ends to +5V and ground
   * wiper to LCD VO pin (pin 3)

The circuit (Button & RGB light):
 * SW1 push button pin to digital pin 22
 * D1 pin 2 to digital pin 28
 * D1 pin 3 to digital pin 25
 * D1 pin 4 to digital pin 24
*/

struct PinConfig { // Objects specific to the board's I/O pin layout and configuration.
  // Defining the variables for the digital pin I/O on LCD
  const int rs = 12, en = 11;
  const int d4 = 5, d5 = 4, d6 = 3, d7 = 2;

  // Defining the variables for the digital pin I/O on RGB & Button
  const int pushButton = 22, r = 28, g = 25, b = 24;
} pin;

struct ButtonConfig { // Objects specific to the functionality and features with the button.
  // Defining variable for button presses & time calculations
  bool isPressed; // boolean value for whether or not the button is pressed
  unsigned long lastPressTime = 0; // to track the time of the last button press
  const unsigned long debounceDelay = 50; // debounce time in milliseconds
  unsigned long pressStartTime = 0; // stores the time when the button was pressed
  unsigned long pressDuration = 0; // stroes the duration of the button press
  unsigned long pressNotDuration = 0; // stores duration of no press on button
  char shortPress = '0'; // defines the short press as char 0 
  char longPress = '1'; // defines the long press as char 1
} button;

LiquidCrystal lcd(pin.rs, pin.en, pin.d4, pin.d5, pin.d6, pin.d7); // defining the LCD screen pins

struct MorseCode { // Declares a dictionary which is used to store the specific key (morse code pattern) and value (letter).
  String key;
  String value;
} alphabet[26]; // An array map to hold the key value pairs.

int mapSize = 0; // keeps track of the current number of elements (key-value pairs) in the alphabet[]

std::vector<char> morseCodeInput; // Collects the user's input from the button for morse code detection.

// Adds a new key-value pair to the 'alphabet' array map.
void add(String key, String value) {
  alphabet[mapSize].key = key; // assigns provided key to the key field at current index (mapSize)
  alphabet[mapSize].value = value; // assigns provided value to the associated key in the value field at current index (mapSize)
  mapSize++; // after new key-value is added, increments by 1 (essentially moving index to next available spot in the array)
}

// Searches and fetches for the key in the 'alphabet' and returns its value if found.
String get(String key) {
  for (int i = 0; i < mapSize; i++) { // loops through each index in 'alphabet' 
    if (alphabet[i].key == key) {  // checks if the key is a match
      return alphabet[i].value; // returns the correct value corresponding to the key
    }
  }
  return "Invalid Code"; // if the key did not match anything
}

String code[26] = {  // Array of size 26 containing all code patterns; in order of A-Z
  "01",
  "1000",
  "1010",
  "100",
  "0",
  "0010",
  "110",
  "0000",
  "00",
  "0111",
  "101",
  "0100",
  "11",
  "10",
  "111",
  "0110",
  "1101",
  "010",
  "000",
  "1",
  "001",
  "0001",
  "011",
  "1001",
  "1011",
  "1100"
};
String letter[26] = { // Array of size 26 containing each letter of the alphabet; in order of A-Z
  "A", 
  "B", 
  "C", 
  "D", 
  "E", 
  "F", 
  "G", 
  "H", 
  "I", 
  "J", 
  "K", 
  "L", 
  "M", 
  "N", 
  "O", 
  "P", 
  "Q", 
  "R", 
  "S", 
  "T", 
  "U", 
  "V", 
  "W", 
  "X", 
  "Y", 
  "Z"
};

// Function to setup the digital pins for I/O
void pin_setup() {
  // Initializes the digitapin.l board pins for I/O
  pinMode(pin.pushButton, INPUT); // sets button to read input
  pinMode(pin.r, OUTPUT); // sets RGB light pins to output
  pinMode(pin.g, OUTPUT);
  pinMode(pin.b, OUTPUT);
  int i; // initializing the pin variable for the board
  for (i = pin.d4; i <= pin.d7; i++)
  {
    pinMode(i, OUTPUT);
  }
}

// sets the color of the RGB light
void set_color(int R, int G, int B) { 
  analogWrite(pin.r, R);
  analogWrite(pin.b, B);
  analogWrite(pin.g, G);
}

// Runs only once when the board turns on
void setup() { 
  pin_setup(); // runs function to setup pins
  
  // Adding the appropriate key-value pairs for the morse code and alphabet
  for (int i = 0; i < 26; i++) {
    add(code[i], letter[i]);
  };

  // Setting up the LCD settings
  lcd.begin(16, 2); // defines number of columns, rows
  lcd.leftToRight(); // go left for the next letters
  // Turn off the display:
  lcd.noDisplay();
  delay(500);
  // Turn on the display:
  lcd.display();
  delay(500);
}

// Runs the whole time when the board is on
void loop() { 
  if (digitalRead(pin.pushButton) == HIGH) { // if the button is pressed
    if (!button.isPressed) { // ensures this is the first time the button is pressed
      button.isPressed = true;
      button.pressNotDuration = millis() - button.lastPressTime; // calculates the duration when button is not pressed
      button.pressStartTime = millis(); // record the time when the button was pressed
    }
  } else { // if the button was not pressed
    if (button.isPressed) { // if the button was previously pressed
      button.isPressed = false;
      button.lastPressTime = millis(); // records time when button is released
      button.pressDuration = millis() - button.pressStartTime; // calculate the press duration
    }
  }

  if (button.pressDuration <= 500) {
    if (button.pressDuration <= 250) {
      
    } else {
      
    }
  } else {
    
  }

  lcd.setCursor(0, 1); // resets cursor to column 0, line 1
}