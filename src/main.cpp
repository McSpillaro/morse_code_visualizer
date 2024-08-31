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
#include "../lib/display.hpp"
#include "../lib/morse_code.hpp"
#include "../lib/rgb.hpp"

struct PinConfiguation
{ // Objects specific to the board's I/O pin layout and configuration.
  // Defining the variables for the digital pin I/O on LCD and RGB light.
  const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
  // Defining the variables for the digital pin I/O on RGB & Button.
  const int pushButton = 7, r = 10, g = 9, b = 6;
} pin;

const int MAX_LCD_SLOTS = 17; // the max amount of slots for a single lcd row
struct LcdConfiguration
{                                 // Sets up the LCD to have predefined lines for buffering
  char line0[MAX_LCD_SLOTS] = ""; // Array containing characters which will be displayed on the first row of the lcd.
  char line1[MAX_LCD_SLOTS] = ""; // Array containing characters which will be displayed on the second row of the lcd.
} lcd_config;
LiquidCrystal lcd(pin.rs, pin.en, pin.d4, pin.d5, pin.d6, pin.d7); // Defines the lcd based on its pins.

// Runs only once when the board turns on. Initializes the pins and sets up board to properly run.
void setup()
{
  Button button; // Creates instance of the class 'Button'

  Serial.begin(9600); // Initialize serial communication at 9600 bits per second

  // Initializes the digital board pins for I/O
  pinMode(pin.pushButton, INPUT); // sets button to read input
  pinMode(pin.r, OUTPUT);         // Sets red rgb pin to output
  pinMode(pin.g, OUTPUT);         // Sets green rgb pin to output
  pinMode(pin.b, OUTPUT);         // Sets blue rgb pin to output
  for (int i = pin.d4; i <= pin.d7; i++)
  {                     // Iterating pin setup for lcd
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
  Button::calc_press;
}