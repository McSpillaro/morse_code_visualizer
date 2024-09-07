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

#include "../lib/button.hpp"
#include "../lib/lcd.hpp"
#include "../lib/morse_code.hpp"
#include "../lib/rgb.hpp"

// Defining the variables for the digital pin I/O on LCD and RGB light.
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
// Defines the lcd based on the pins it is connected to.
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Creating instances of classes from header files.
struct ClassInstances
{
  Button button;                                  // Creates instance of the class 'Button'
  Properties::ButtonProperties button_properties; // Creates instance of 'Properties' from button.hpp
  MorseCode morseCode;                            // Creates instance of the class 'MorseCode'
  Data::InputArray userData;                      // Creates instance of the class 'Data'
  Display display;                                // Creates instance of the 'Display' class in display.hpp
  Light indicator;                                // Instance of 'Light' class from rgb.hpp and handles rgb light indication signals.
} Instance;

// Runs only once when the board turns on. Initializes the pins and sets up board to properly run.
void setup()
{
  Serial.begin(9600); // Initialize serial communication at 9600 bits per second

  // Initializes the digital board pins for I/O
  pinMode(Instance.button.buttonPin, INPUT); // sets button to read input
  pinMode(Instance.indicator.r, OUTPUT);     // Sets red rgb pin to output
  pinMode(Instance.indicator.g, OUTPUT);     // Sets green rgb pin to output
  pinMode(Instance.indicator.b, OUTPUT);     // Sets blue rgb pin to output

  for (int i = d4; i <= d7; i++) // Iterating pin setup for lcd
  {
    pinMode(i, OUTPUT); // Sets each digital pin for lcd to output
  }

  // Initializes the lcd
  lcd.begin(16, 2);  // Defines number of columns, rows
  lcd.leftToRight(); // Sets default reading/writing pattern
  lcd.display();     // Turns on the display

  // Serial output
  Serial.println("Setup complete.");
}

// Constantly runs when the board turns on.
void loop()
{
  Instance.button.calc_press();     // Constantly checks for button press/release
  Instance.morseCode.check_press(); // Checks for a valid short or long press based on the data and input durations (press/release)

  if (strlen(Instance.userData.userInput) == 4 || Instance.button_properties.inputFinal)
  {
    if (Instance.morseCode.check_pattern()) // Will change the display arrays if a pattern condition was successful found
    {
      Instance.display.update_display(lcd, Instance.morseCode.patternResult);
      Instance.indicator.color(LOW, HIGH, LOW); // Sets rgb indicator to green
    }
    else
    {
      Instance.indicator.color(HIGH, LOW, LOW); // Sets rgb indicator to red
    };
  };

  Instance.indicator.off(); // Turns off rgb indicator at the end of everything
}