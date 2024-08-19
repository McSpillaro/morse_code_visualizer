/*
LiquidCrystalDisplay (LCD) - Morse Code

Displays the inputted morse code letters on the LCD panel.
This sketch takes input from a button and displays proper characters
that correspond to the proper morse code.

After holding the button for 2 seconds, it will clear the display.

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
 * SW1 push button pin to digital pin 7
 * D1 pin 2 to digital pin 13
 * D1 pin 3 to digital pin 10
 * D1 pin 4 to digital pin 9
*/

// All libraries are installed from PlatformIO libraries onto the project (not sys dependent)
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <ArduinoSTL.h>
#include <vector>
#include <string>

struct PinConfig { // Objects specific to the board's I/O pin layout and configuration.
  // Defining the variables for the digital pin I/O on LCD
  const int rs = 12, en = 11;
  const int d4 = 5, d5 = 4, d6 = 3, d7 = 2;

  // Defining the variables for the digital pin I/O on RGB & Button
  const int pushButton = 7, r = 6, g = 10, b = 9;
} pin;

struct ButtonConfig { // Objects specific to the functionality and features with the button.
  // Defining variable for button presses & time calculations

  /*
  Important moments of button functionality that needs to be tracked:
    * initial button press
    * initial button release
    * next button press
    * next button release
  */

  bool isPressed; // boolean value for whether or not the button is pressed
  unsigned long currentPressTime = 0; // to track the time of the current button press
  unsigned long currentReleaseTime = 0; // to track the time of the button the moment it is released
  unsigned long lastPressTime = 0; // to track the time of the last button press
  unsigned long lastReleaseTime = 0; // tracks last time the button was released
  unsigned long pressDuration = 0; // stores the duration of the button press
  unsigned long releaseDuration = 0; // stores duration of button not being pressed
  const unsigned long debounceDelay = 50; // debounce time in milliseconds
  char shortPress = '0'; // defines the short press as char 0 
  char longPress = '1'; // defines the long press as char 1
} button;

struct LCDConfig { // Sets up the LCD to have predefined lines for buffering
  char line0[21];
  char line1[21];
} lcd_config;
LiquidCrystal lcd(pin.rs, pin.en, pin.d4, pin.d5, pin.d6, pin.d7); // defining the LCD screen pins

struct Output { // All output on the board like the LCD screen
  String morseCodeOutput; // letter for corresponding morse code
} userOutput;

struct MorseCode { // Declares a dictionary which is used to store the specific key (morse code pattern) and value (letter).
  String key; // morse code
  String value; // corresponding letter
} alphabet[26]; // An array map to hold the key-value pairs.

int mapSize = 0; // keeps track of the current number of elements (key-value pairs) in the alphabet[]

std::vector<char> morseCodeInput; // Collects the user's input from the button for morse code detection.
String morseCodeJoined; // Takes the user input and then joins them as one string

// Adds a new key-value pair to the 'alphabet' array map.
void add(String key, String value) {
  alphabet[mapSize].key = key; // assigns provided key to the key field at current index (mapSize)
  alphabet[mapSize].value = value; // assigns provided value to the associated key in the value field at current index (mapSize)
  mapSize++; // after new key-value is added, increments by 1 (essentially moving index to next available spot in the array)
}

// Searches and fetche     s for the key in the 'alphabet' and returns its value if found.
String get(String key) {
  for (int i = 0; i < mapSize; i++) { // loops through each index in 'alphabet' 
    if (alphabet[i].key == key) {  // checks if the key is a match
      return alphabet[i].value; // returns the correct value corresponding to the key
    }
  }
  return "Invalid Code"; // if the key did not match anything
}

String combine(std::vector<char> input) { // Combines the separated indexes of the input into one string
  morseCodeJoined = ""; // Ensure it's cleared before use
  for (char c : input) { // iterates over the user morse code input
    morseCodeJoined += c; // joins the separated morse code inputs into one string code
  }
  String result = get(morseCodeJoined);
  return result;
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
  // Initializes the digital board pins for I/O
  pinMode(pin.pushButton, INPUT); // sets button to read input
  pinMode(pin.r, OUTPUT); // sets RGB light pins to output
  pinMode(pin.g, OUTPUT);
  pinMode(pin.b, OUTPUT);
  for (int i = pin.d4; i <= pin.d7; i++) { // Iterating pin setup for LCD pins
    pinMode(i, OUTPUT);
  }
}

// Function to setup the LCD
void lcd_setup() {
  // Setting up the LCD settings
  lcd.begin(16, 1); // defines number of columns, rows
  lcd.leftToRight(); // go left for the next letters
  // Turn off the display:
  lcd.noDisplay();
  delay(500);
  // Turn on the display:
  lcd.display();
}

// Updates the display of the LCD including the buffer
void update_display() {
  lcd.setCursor(0, 0); // sets cursor to default position
  lcd.print(lcd_config.line0); // handles actual printing
  lcd.print(lcd_config.line1); // ...
}

// Sets the color of the RGB light
void set_color(int R, int G, int B, int A) { 
  analogWrite(pin.r, R);
  analogWrite(pin.b, B);
  analogWrite(pin.g, G);
}

// Checks for valid button presses
void check_press() {
  button.currentPressTime = millis(); // gets the current time 
  
  // general defining of necessary variables regarding button pressing times and calculations
  if (digitalRead(pin.pushButton) == HIGH) { // if button is pressed at all
    if (!button.isPressed && (button.currentPressTime - button.lastPressTime > button.debounceDelay)) {  // checks that this is the first time the button is pressed
      button.isPressed = true; // sets pressed marker to 'true'
      button.lastPressTime = button.currentPressTime; // update the time of the last press time
      button.lastReleaseTime = button.currentReleaseTime; // updates time track of last release time after new button press
    }
    else
    {
      button.isPressed = false; // sets marker to 'false' when button is not pressed OR when debounce is checked
      button.currentReleaseTime = millis(); // tracks time the moment of button release
    }
  }

  if (button.isPressed) { // only for setting the light to white when pressed
    set_color(255, 255, 255, 1); // rgb light indicator for presses -> r#, g#, b#
  } else {
    set_color(0, 0, 0, 0); // default turns off the RGB light indicator -> r#, g#, b#
  }

  // calculating the duration of presses and releases for morse code
  button.pressDuration = button.currentReleaseTime - button.currentPressTime; // calculates duration based on 'current' vars
  button.releaseDuration = button.currentPressTime - button.lastReleaseTime; // calculates release duration based on 'current' press and 'last' release
}

// Runs only once when the board turns on
void setup() {
  // Adding the appropriate key-value pairs for the morse code and alphabet
  for (int i = 0; i < 26; i++) {
    add(code[i], letter[i]);
  }

  pin_setup(); // runs function to setup pins
  lcd_setup(); // runs function to setup lcd
}

// Runs the whole time when the board is on
void loop() { 
  morseCodeInput.clear(); // clears the user's input in array
  morseCodeJoined = ""; // clears the content of any joined chars from morseCodeInput
  check_press();
  update_display();
}