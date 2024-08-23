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
 * D1 pin 2 to digital pin 10
 * D1 pin 3 to digital pin 9
 * D1 pin 4 to digital pin 6
*/

// All libraries are installed from PlatformIO libraries onto the project (not sys dependent)
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <avr/pgmspace.h>

struct PinConfig { // Objects specific to the board's I/O pin layout and configuration.
  // Defining the variables for the digital pin I/O on LCD and RGB light
  const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
  // Defining the variables for the digital pin I/O on RGB & Button
  const int pushButton = 7, r = 10, g = 9, b = 6;
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

  bool isPressed = false; // boolean value for whether or not the button is pressed
  unsigned long currentPressTime = 0; // to track the time of the current button press
  unsigned long currentReleaseTime = 0; // to track the time of the button the moment it is released
  unsigned long lastPressTime = 0; // to track the time of the last button press
  unsigned long lastReleaseTime = 0; // tracks last time the button was released
  unsigned long pressDuration = 0; // stores the duration of the button press
  unsigned long releaseDuration = 0; // stores duration of button not being pressed
  const unsigned long debounceDelay = 50; // debounce time in milliseconds
  const unsigned long clearScreenThreshold = 2000; // hold button for 2 seconds to clear lcd

  char shortPress = '0'; // defines the short press as char 0 
  char longPress = '1'; // defines the long press as char 1

  const int shortPressCap = 100; // Hard cap limit (100 ms) for detecting a short press
  const int longPressCap = 300; // Hard cap limit (300 ms) for detecting a long press
  float avgPressDuration = 0.; // defines the average press duration for total button presses
  float stdPressDuration = 0.; // defines the standard deviation duration for all button presses
  float avgReleaseDuration = 0.; // defines the average release duration for total releases
  float stdReleaseDuration = 0.; // defines the standard deviation release duration for total releases
} button;

const int MAX_LCD_SLOTS = 17; // the max amount of slots for a single lcd row
struct LCDConfig { // Sets up the LCD to have predefined lines for buffering
  char line0[MAX_LCD_SLOTS] = ""; // extra index 16+1 for the null char at the end
  char line1[MAX_LCD_SLOTS] = ""; // ...
} lcd_config;
LiquidCrystal lcd(pin.rs, pin.en, pin.d4, pin.d5, pin.d6, pin.d7); // defining the LCD screen pins

struct Output { // All output on the board like the LCD screen
  char morseCodeOutput; // letter for corresponding morse code
} userOutput;

// Morse code and alphabet stored in program memory
const char* const morseCode[] PROGMEM = {
  "01", "1000", "1010", "100", "0", "0010", "110", "0000", 
  "00", "0111", "101", "0100", "11", "10", "111", "0110", 
  "1101", "010", "000", "1", "001", "0001", "011", "1001", 
  "1011", "1100"
};
const char alphabet[] PROGMEM = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

const int MAX_INPUT_SIZE = 5; // Max input size for morse code (4), which includes the buffer (1)

// Stores morse code input
char morseCodeInput[MAX_INPUT_SIZE] = ""; 
// Morse code duration arrays
int pressDurations[MAX_INPUT_SIZE - 1] = {};
int releaseDurations[MAX_INPUT_SIZE - 1] = {};

bool addToMorseCode(char* array, char newChar) { // Adds corresponding short press '0' or long press '1' to the array for holding morse code press patterns before defining a specific character letter to output
  int length = strlen(array);
  if (length < MAX_INPUT_SIZE - 2) { // Ensure there is space for the new character and null terminator
    array[length] = newChar; // Adds the new character to the string
    array[length + 1] = '\0'; // Null-terminate the string
    Serial.print("Added to morseCodeInput: ");
    Serial.println(newChar);
    return true; // Successfully added
  } else {
    Serial.println("morseCodeInput array is full.");
    return false; // The array is full
  }
}

bool addToDurationArray(int* array, int dataPoint) { // Adds press or release duration data point to specified array
  int length = sizeof(array); // gets length of array by size
  if (length < MAX_INPUT_SIZE - 2) // Ensures there is space for the new datapoint
  {
    array[length] = dataPoint; // Adds the data point to the array
    Serial.print("Added to duration array: ");
    Serial.println(dataPoint);
    return true; // Successfully added datapoint
  } else {
    Serial.println("Duration array is full.");
    return false; // The array is full of data points
  }
}

// Function to get the corresponding letter for a Morse code from program memory
char getLetterFromMorse(char* code) {
  for (int i = 0; i < 26; i++) { // Loops through alphabet[] array
    if (strcmp_P(code, (char*)pgm_read_word(&(morseCode[i]))) == 0)
    {                                       // Compares RAM-based 'code' string to 'alphabet' flash memory string; returns 0 if a match
      Serial.print("Morse code ");
      Serial.print(code);
      Serial.print(" matches letter ");
      Serial.println((char)pgm_read_byte(&(alphabet[i])));
      return pgm_read_byte(&(alphabet[i])); // Address of the morse code string for the letter at index 'i' in the 'morseCode' memory
    }
  }
  Serial.print("Morse code ");
  Serial.print(code);
  Serial.println(" does not match any letter.");
  return '?'; // Return '?' if no match found
}

// Function to setup the digital pins for I/O
void pin_setup() {
  // Initializes the digital board pins for I/O
  pinMode(pin.pushButton, INPUT); // sets button to read input
  pinMode(pin.r, OUTPUT); // sets RGB light pins to output
  pinMode(pin.g, OUTPUT); // ...
  pinMode(pin.b, OUTPUT); // ...
  for (int i = pin.d4; i <= pin.d7; i++) { // Iterating pin setup for LCD pins
    pinMode(i, OUTPUT);
  }
}

// Function to setup the LCD
void lcd_setup() {
  // Setting up the LCD settings
  lcd.begin(16, 2); // defines number of columns, rows
  lcd.leftToRight(); // go left for the next letters
  // Turn off the display:
  lcd.noDisplay();
  delay(500);
  // Turn on the display:
  lcd.display();
  Serial.println("LCD setup complete.");
}

void lcd_scroll(char letter) {
  int length0 = strlen(lcd_config.line0); // length of the array for the top row on the lcd
  int length1 = strlen(lcd_config.line1); // length of the array for the bottom row on the lcd
  
  if (length0 == 0) { // if the lcd is completely fresh/blank; runs here and stops
    lcd_config.line0[0] = letter; // updates the 1st column, 1st row slot with the letter
  } else if (length0 > 0 && length0 < 16) { // if there is room in the first row to add the letter
    lcd_config.line0[length0] = letter; // updates the next open slot with the new letter
  } else if (length0 == 16 && length1 == 0) { // if the first row is full but nothing in the second row; runs here and stops
    lcd_config.line1[0] = letter; // updates the 1st column, 2nd row slot with the letter
  } else if (length1 > 0 && length1 < 16) { // if there is room in the second row to add the letter
    lcd_config.line1[length1] = letter; // updates the next open slot with the new letter
  } else { // if the entire lcd screen is full; scroll down the lcd screen and add the new letter
    strcpy(lcd_config.line0, lcd_config.line1); // top row copies bottom row's letters
    strcpy(lcd_config.line1, ""); // bottom row is wiped blank
    lcd_config.line1[0] = letter; // updates the 1st column, 2nd row slot with the letter
  }

  Serial.print("LCD Update: ");
  Serial.print(lcd_config.line0);
  Serial.print(" | ");
  Serial.println(lcd_config.line1);
  
  lcd.clear(); // wipes the lcd screen clean and reset to add the next string of letters
  lcd.setCursor(0, 0); // sets cursor at the 1st column, 1st row
  lcd.print(lcd_config.line0); // prints letters in memory for the top row
  lcd.setCursor(0, 1); // sets cursor at the 1st column, 2nd row
  lcd.print(lcd_config.line1); // prints letters in memory for the bottom row
}

void rgb_light(String color) { // Activates the rgb led
  // Turns off all lights first
  digitalWrite(pin.r, LOW);
  digitalWrite(pin.g, LOW);
  digitalWrite(pin.b, LOW);

  // Define the light color by setting the appropriate LED pins
  if (color == "red") {
    digitalWrite(pin.r, HIGH);
  } else if (color == "green") {
    digitalWrite(pin.g, HIGH);
  } else if (color == "blue") {
    digitalWrite(pin.b, HIGH);
  } else if (color == "yellow") {
    digitalWrite(pin.r, HIGH);
    digitalWrite(pin.g, HIGH);
  } else if (color == "cyan") {
    digitalWrite(pin.g, HIGH);
    digitalWrite(pin.b, HIGH);
  } else if (color == "magenta") {
    digitalWrite(pin.r, HIGH);
    digitalWrite(pin.b, HIGH);
  } else if (color == "white") {
    digitalWrite(pin.r, HIGH);
    digitalWrite(pin.g, HIGH);
    digitalWrite(pin.b, HIGH);
  }

  Serial.print("RGB Light set to ");
  Serial.println(color);
}

void check_input() {
  // Check if the button is currently pressed
  bool currentState = digitalRead(pin.pushButton) == HIGH;

  if (currentState && !button.isPressed) {
    // Button was just pressed
    button.isPressed = true; // sets isPressed flag to true
    button.currentPressTime = millis(); // sets the time of current press
    button.pressDuration = button.currentPressTime - button.lastReleaseTime; // calculates duration between this press and the last release

    Serial.print("Button pressed. Press duration: ");
    Serial.println(button.pressDuration);

    // Add press duration to the press durations array
    addToDurationArray(pressDurations, button.pressDuration);

    // Check for short press or long press
    if (button.pressDuration < button.shortPressCap) {
      // Short press detected
      addToMorseCode(morseCodeInput, button.shortPress);
    } else {
      // Long press detected
      addToMorseCode(morseCodeInput, button.longPress);
    }
  } else if (!currentState && button.isPressed) {
    // Button was just released
    button.isPressed = false; // sets isPressed flag to false
    button.currentReleaseTime = millis(); // sets the time of current release
    button.releaseDuration = button.currentReleaseTime - button.currentPressTime; // calculates duration between this release and the current press

    Serial.print("Button released. Release duration: ");
    Serial.println(button.releaseDuration);

    // Add release duration to the release durations array
    addToDurationArray(releaseDurations, button.releaseDuration);

    // Check if the release duration is long enough to end a character input
    if (button.releaseDuration > button.longPressCap) {
      userOutput.morseCodeOutput = getLetterFromMorse(morseCodeInput); // Get corresponding letter for the inputted morse code
      lcd_scroll(userOutput.morseCodeOutput); // Display the output letter
      rgb_light("green"); // Set RGB light to green
      morseCodeInput[0] = '\0'; // Reset morse code input buffer
      memset(pressDurations, 0, sizeof(pressDurations)); // Reset press durations array
      memset(releaseDurations, 0, sizeof(releaseDurations)); // Reset release durations array
    }

    // Update last press and release times
    button.lastPressTime = button.currentPressTime;
    button.lastReleaseTime = button.currentReleaseTime;
  }

  // Check if button is being held down to clear the LCD screen
  if (currentState && (millis() - button.currentPressTime >= button.clearScreenThreshold)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LCD Cleared");
    rgb_light("red"); // Set RGB light to red
    delay(500);
    lcd.clear();
    button.isPressed = false;
    Serial.println("LCD screen cleared.");
  }
}

void setup() {
  Serial.begin(9600); // Initialize serial communication at 9600 bits per second
  pin_setup(); // Call pin setup function
  lcd_setup(); // Call LCD setup function
  Serial.println("Setup complete.");
}

void loop() {
  check_input(); // Check input from the button
}