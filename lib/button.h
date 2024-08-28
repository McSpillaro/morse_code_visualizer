#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
#include "durations.h"

class Button {// Handles structures and functions regarding button presses.

    private:
    Durations::ButtonProperties button_properties; // Duration, state, and defintion properties of a button.

    public: // Allows all objects in class to be used by other project files.
    
    // Function to handle detecting valid button presses and releases. Also calculates whether or not the button is pressed or released for x amount of time (ms).
    void check_press(int buttonPin) {
        button_properties.currentPressTime = millis(); // gets the current time

        // general defining of necessary variables regarding button pressing times and calculations
        if (digitalRead(buttonPin) == HIGH) { // if button is pressed at all
            if (!button_properties.isPressed && (button_properties.currentPressTime - button_properties.lastPressTime > button_properties.debounceDelay)) {  // checks that this is the first time the button is pressed
            button_properties.isPressed = true; // sets pressed marker to 'true'
            button_properties.lastPressTime = button_properties.currentPressTime; // update the time of the last press time
            button_properties.lastReleaseTime = button_properties.currentReleaseTime; // updates time track of last release time after new button press
            } else {
            button_properties.isPressed = false; // sets marker to 'false' when button is not pressed OR when debounce is checked
            button_properties.currentReleaseTime = millis(); // tracks time the moment of button release
            }
        }

        // Calculates current individual press and release durations. 
        button_properties.pressDuration = button_properties.currentReleaseTime - button_properties.currentPressTime; // calculates duration based on 'current' vars
        button_properties.releaseDuration = button_properties.currentPressTime - button_properties.lastReleaseTime; // calculates release duration based on 'current' press and 'last' release
    };
};

#endif // BUTTON_H