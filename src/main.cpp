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

#include "../lib/button.h"
#include "../lib/display.h"
#include "../lib/morse_code.h"
#include "../lib/rgb.h"

using namespace std;

struct PinConfig { // Objects specific to the board's I/O pin layout and configuration.
  // Defining the variables for the digital pin I/O on LCD and RGB light
  const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
  // Defining the variables for the digital pin I/O on RGB & Button
  const int pushButton = 7, r = 10, g = 9, b = 6;
} pin;

const int MAX_LCD_SLOTS = 17; // the max amount of slots for a single lcd row
struct LCDConfig { // Sets up the LCD to have predefined lines for buffering
  char line0[MAX_LCD_SLOTS] = ""; // extra index 16+1 for the null char at the end
  char line1[MAX_LCD_SLOTS] = ""; // ...
} lcd_config;
LiquidCrystal lcd(pin.rs, pin.en, pin.d4, pin.d5, pin.d6, pin.d7); // defining the LCD screen pins

struct Output { // All output on the board like the LCD screen
  char morseCodeOutput; // letter for corresponding morse code
} userOutput;

const int MAX_INPUT_SIZE = 5; // Max input size for morse code (4), which includes the buffer (1)

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

// Sets the color of the RGB light where rgb(LOW-HIGH, LOW-HIGH, LOW-HIGH)
void set_color(uint8_t R, uint8_t G, uint8_t B) { 
  digitalWrite(pin.r, R);
  digitalWrite(pin.b, B);
  digitalWrite(pin.g, G);
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
    set_color(HIGH, HIGH, HIGH); // sets rgb light to white if a press is confirmed
  } else {
    set_color(LOW, LOW, LOW); // turns off the rgb light
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
  if ((button.releaseDuration > button.avgReleaseDuration + thresholdMultiplier * button.stdReleaseDuration || strlen(morseCodeInput) == MAX_INPUT_SIZE - 1) & morseCheckResult != '?') {
    // Long release duration is for a pause between character inputs (different from individual press)
    userOutput.morseCodeOutput += morseCheckResult; // convert the current morse code to a letter based on pattern of morse code that was input into ''morseCodeInput' array
    set_color(LOW, HIGH, LOW); // sets rgb light to green if a valid morse code combination was detected
    clear_input(); // Clear the input to start the next character
  } else {
    set_color(HIGH, LOW, LOW); // sets rgb light to red if invalid morse code combination was detected
    clear_input(); // clears the input to start processing next character
  }

  if (button.pressDuration > button.clearScreenThreshold) {
    lcd.clear(); // clears the lcd
    set_color(LOW, LOW, HIGH); // sets rgb light to blue if the screen is being cleared
  }
}

// Runs only once when the board turns on
void setup() {
  Serial.begin(9600); // Initialize serial communication at 9600 bits per second
  
  // Initializes the digital board pins for I/O
  pinMode(pin.pushButton, INPUT); // sets button to read input
  pinMode(pin.r, OUTPUT); // Sets red rgb pin to output
  pinMode(pin.g, OUTPUT); // Sets green rgb pin to output
  pinMode(pin.b, OUTPUT); // Sets blue rgb pin to output
  for (int i = pin.d4; i <= pin.d7; i++) { // Iterating pin setup for lcd
    pinMode(i, OUTPUT); // Sets each digital pin for lcd to output
  }
  
  // Initializes the lcd
  lcd.begin(16, 2); // Defines number of columns, rows
  lcd.leftToRight(); // Sets default reading/writing pattern
  lcd.display(); // Turns on the display
  
  // Serial output
  Serial.println("Setup complete.");
}

void loop() {
  check_input(); // Check input from the button
}