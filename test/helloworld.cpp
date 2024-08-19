#include <Arduino.h>

void setup() {
  // Initialize serial communication at a baud rate of 9600
  Serial.begin(9600);
}

void loop() {
  // Print "Hello, World!" to the serial monitor
  Serial.println("Hello, World!");
  delay(1000); // Wait for 1 second
}
