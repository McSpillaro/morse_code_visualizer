#include <LiquidCrystal.h>
#include <Arduino.h>

// LCD pins: RS, EN, D4, D5, D6, D7
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Button and RGB pins
const int buttonPin = 7;
const int redPin = 8;
const int greenPin = 9;
const int bluePin = 10;

// Morse code timing
unsigned long pressStartTime;
unsigned long releaseStartTime;
unsigned long pressDuration;
unsigned long releaseDuration;

// Morse code input array
const int morseCodeLength = 10;
char morseCodeInput[morseCodeLength + 1] = ""; // +1 for null terminator
int durationArray[morseCodeLength];
int inputIndex = 0;

void setup() {
  // Setup LCD
  lcd.begin(16, 2);
  lcd.print("LCD setup complete.");
  Serial.begin(9600);
  Serial.println("LCD setup complete.");

  // Setup button and RGB pins
  pinMode(buttonPin, INPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  // Setup complete
  Serial.println("Setup complete.");
}

void loop() {
  checkInput(); // Check for button press and release
}

// CHECK BUTTON PRESS DURATION AND ADD TO ARRAY
void checkInput() {
  if (digitalRead(buttonPin) == HIGH) { // Button pressed
    pressStartTime = millis();
    while (digitalRead(buttonPin) == HIGH) {
      // WAIT FOR BUTTON RELEASE
    }
    pressDuration = millis() - pressStartTime;
    Serial.print("Button pressed. Press duration: ");
    Serial.println(pressDuration);

    if (inputIndex < morseCodeLength) {
      durationArray[inputIndex] = pressDuration;
      morseCodeInput[inputIndex] = '1'; // TEMPORARY PLACEHOLDER
      inputIndex++;
      Serial.print("Added to duration array: ");
      Serial.println(pressDuration);
      Serial.print("Added to morseCodeInput: ");
      Serial.println('1');
    } else {
      Serial.println("morseCodeInput array is full.");
    }

    setRGBColor(255, 0, 0); // SET RGB LIGHT TO RED
    lcd.clear();
  }
}

// SET RGB LIGHT COLOR
void setRGBColor(int redValue, int greenValue, int blueValue) {
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);
}

// RESET MORSE CODE INPUT ARRAY
void resetMorseCodeInput() {
  memset(morseCodeInput, 0, sizeof(morseCodeInput)); // CLEAR THE ARRAY
  inputIndex = 0; // RESET THE INDEX
}
