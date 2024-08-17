#include <Arduino.h>
#include <LiquidCrystal.h>
#include <ArduinoSTL.h>
#include <vector>

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

// Defining the variables for the digital pin I/O on LCD
const int rs = 12, en = 11;
const int d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Defining the variables for the digital pin I/O on RGB & Button
const int button = 22, r = 28, g = 25, b = 24;

// Defining variable for button presses & time calculations
bool isPressed; // boolean value for whether or not the button is pressed
unsigned long lastPressTime = 0; // to track the time of the last button press
const unsigned long debounceDelay = 50; // debounce time in milliseconds
unsigned long pressStartTime = 0; // stores the time when the button was pressed
unsigned long pressDuration = 0; // stroes the duration of the button press
unsigned long pressNotDuration = 0; // stores duration of no press on button
char shortPress = '0'; // defines the short press as char 0 
char longPress = '1'; // defines the long press as char 1

/*
Using an array of values 0 and 1 to define short and long presses
0 -> short press
1 -> long press

The collection of 0101... will define a letter that is output
onto the LCD
*/

int morseCode[] = { // defining the array containing all morse code letters
  '01'
};

// Setting up the array to collect user input
std::vector<char> morseCodeInput;

// Function to setup the digital pins for I/O
void pin_setup() {
  // Initializes the digital board pins for I/O
  pinMode(button, INPUT); // sets button to read input
  pinMode(r, OUTPUT); // sets RGB light pins to output
  pinMode(g, OUTPUT);
  pinMode(b, OUTPUT);
  int pin; // initializing the pin variable for the board
  for (pin = d4; pin <= d7; pin++)
  {
    pinMode(pin, OUTPUT);
  }
}

void set_color(int R, int G, int B) { // sets the color of the RGB light
  analogWrite(r, R);
  analogWrite(b, B);
  analogWrite(g, G);
}

void setup() { // runs only once when the board turns on
  pin_setup(); // runs function to setup pins
  
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

void loop() { // runs the whole time when the board is on

  /*
  Line 103-111 checks to see if the button is pressed or not and
  calculates the duration of the press for morse code
  */

  if (digitalRead(button) == HIGH) { // if the button is pressed
    if (!isPressed) { // ensures this is the first time the button is pressed
      isPressed = true;
      pressNotDuration = millis() - lastPressTime; // calculates the duration when button is not pressed
      pressStartTime = millis(); // record the time when the button was pressed
    }
  } else { // if the button was not pressed
    if (isPressed) { // if the button was previously pressed
      isPressed = false;
      lastPressTime = millis(); // records time when button is released
      pressDuration = millis() - pressStartTime; // calculate the press duration
    }
  }

  /*
  */

  if (pressDuration <= 500) {
    if (pressDuration <= 250) {
      
    }
  } else {

  }

  lcd.setCursor(0, 1); // resets cursor to column 0, line 1
}