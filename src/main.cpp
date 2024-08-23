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
const char* morseCode[] PROGMEM = {
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
    return true; // Successfully added
  } else {
    return false; // The array is full
  }
}

bool addToDurationArray(int* array, int dataPoint) { // Adds press or release duration data point to specified array
  int length = sizeof(array); // gets length of array by size
  if (length < MAX_INPUT_SIZE - 2) // Ensures there is space for the new datapoint
  {
    array[length] = dataPoint; // Adds the data point to the array
    return true; // Successfully added datapoint
  } else {
    return false; // The array is full of data points
  }
}

// Function to get the corresponding letter for a Morse code from program memory
char getLetterFromMorse(char* code) {
  for (int i = 0; i < 26; i++) { // Loops through alphabet[] array
    if (strcmp_P(code, (char*)pgm_read_word(&(morseCode[i]))) == 0)
    {                                       // Compares RAM-based 'code' string to 'alphabet' flash memory string; returns 0 if a match
      return pgm_read_byte(&(alphabet[i])); // Address of the morse code string for the letter at index 'i' in the 'morseCode' memory
    }
  }
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
}

void lcd_scroll(char letter) {
  int length0 = strlen(lcd_config.line0); // length of the array for the top row on the lcd
  int length1 = strlen(lcd_config.line1); // length of the array for the bottom row on the lcd
  
  if (length0 == 0) { // if the lcd is completely fresh/blank; runs here and stops
    lcd_config.line0[0] = letter; // updates the 1st column, 1st row slot with the letter
  } else {
    for (int i = 0; i <= length0; i++) { // goes through each filled slot in the 1st lcd row
      lcd_config.line0[i+1] = lcd_config.line0[i]; // sets next slot to be the next to be the prev character
      if (i+1 == MAX_LCD_SLOTS) { // if the 1st row of the lcd is full
        lcd_config.line1[0] = lcd_config.line0[i]; // moves last character in 1st row to start of 2nd row
        for (int j = 0; j < length1; j++) { // goes through each filled slot in the 2nd lcd row
          lcd_config.line1[j+1] = lcd_config.line1[j]; // sets next slot to be the prev character
        }
      }
    }
    lcd_config.line0[0] = letter; // updates (0,0);(c,r) with the letter after scrolling update
  }
}

// Updates the display of the LCD including the buffer
void update_display(char letter) {
  lcd_scroll(letter);
  lcd.setCursor(0, 0); // sets cursor to default position
  lcd.print(lcd_config.line0); // handles actual printing
  lcd.setCursor(0, 1);
  lcd.print(lcd_config.line1); // ...
}

// Sets the color of the RGB light where rgb(0-255, 0-255, 0-255)
void set_color(int R, int G, int B) { 
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
    set_color(100, 100, 100); // rgb light indicator for presses -> r#, g#, b#
  } else {
    set_color(0, 0, 0); // default turns off the RGB light indicator -> r#, g#, b#
  }

  // calculating the duration of presses and releases for morse code
  button.pressDuration = button.currentReleaseTime - button.currentPressTime; // calculates duration based on 'current' vars
  button.releaseDuration = button.currentPressTime - button.lastReleaseTime; // calculates release duration based on 'current' press and 'last' release
  addToDurationArray(pressDurations, button.pressDuration); // adds datapoint to array
  addToDurationArray(releaseDurations, button.releaseDuration); // ...
}

float averageArray(int* array) {
  int sum = 0;
  for (int i = 0; i < sizeof(array); i++) { // loops through the array
    sum += array[i]; // adds the datapoint into the array
  }
  float avgResult = sum / sizeof(array); // calculates the average of array
  return avgResult;
}

float stdArray(int* array, float avg) {
  int arraySize = sizeof(array); // gets the size of the array
  float sum = 0.; // sum of the squared datapoint - average of array
  float stdResult = 0; // initialize var to hold the result of standard deviation
  for (int i = 0; i < sizeof(array); i++) { // loops times the size of array
    sum += (array[i] - avg) * (array[i] - avg); // sum of squared data points - average
  }
  stdResult = sqrt(sum / (arraySize - 1)); // calculates standard deviation
  return stdResult;
}

void clear_input() { // Clears the arrays 'morseCodeInput' and '...Duration' by clearing the memory
  memset(morseCodeInput, 0, sizeof(morseCodeInput)); // clears the user's input in array
  memset(releaseDurations, 0, sizeof(releaseDurations)); // clears the data storing the memory for release durations
  memset(pressDurations, 0, sizeof(pressDurations)); // clears the data storing the memory for press durations
}

void check_input() { // Based on 'check_press' vars, defines the different durations of presses/releases
  // Logic to process Morse code input and determine if it's a short or long press
  button.avgPressDuration = averageArray(pressDurations); // calculates avg durations
  button.avgReleaseDuration = averageArray(releaseDurations); // ...
  button.stdPressDuration = stdArray(pressDurations, button.avgPressDuration); // calculates standard deviation durations based on calculated avg
  button.stdReleaseDuration = stdArray(releaseDurations, button.avgReleaseDuration); // ... 
  
  float thresholdMultiplier = 1.5; // threshold for standard deviation (tunable)

  // Determine whether or not the current press is short or long
  if (button.pressDuration <= button.avgPressDuration + thresholdMultiplier * button.stdPressDuration || button.pressDuration < button.shortPressCap) {
    addToMorseCode(morseCodeInput, button.shortPress); // short press
  } else if (button.pressDuration <= button.avgPressDuration + thresholdMultiplier * button.stdPressDuration || button.shortPressCap < button.pressDuration < button.longPressCap) {
    addToMorseCode(morseCodeInput, button.longPress); // long press
  }

  char morseCheckResult = getLetterFromMorse(morseCodeInput); // checks the returned char from the function (actual char if correct code; '?' if not)
  // Determine if the release duration indicates the end of a press vs character
  if (button.releaseDuration > button.avgReleaseDuration + thresholdMultiplier * button.stdReleaseDuration & morseCheckResult != '?') {
    // Long release duration is for a pause between character inputs (different from individual press)
    userOutput.morseCodeOutput += morseCheckResult; // convert the current morse code to a letter based on pattern of morse code that was input into ''morseCodeInput' array
    set_color(0, 100, 0); // sets rgb light to green; valid input
    clear_input(); // Clear the input to start the next character
  } else {
    set_color(100, 0, 0); // sets rgb light to red; invalid input
    clear_input(); // clears the input to start processing next character
  }

  if (button.pressDuration > button.clearScreenThreshold) {
    lcd.clear(); // clears the lcd
    set_color(0, 0, 100); // sets rgb light to blue; clear screen indicator
  }
}

// Runs only once when the board turns on
void setup() {
  pin_setup(); // runs function to setup pins
  lcd_setup(); // runs function to setup lcd
}

// Runs the whole time when the board is on
void loop() { 
  check_press();
  check_input();
  update_display(userOutput.morseCodeOutput);
}