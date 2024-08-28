#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

class Button // Handles structures and functions regarding button presses.
{
    public: // Allows all objects in class to be used by other project files.

    struct Config // Structure which defines the key states and properties of the button.
    {
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
        const int shortPressCap = 100; // 100 ms hardcap to detect a short press
        const int longPressCap = 300; // 300 ms hardcap to detect a long press

        char shortPress = '0'; // defines the short press as char 0 
        char longPress = '1'; // defines the long press as char 1

        float avgPressDuration = 0.; // defines the average press duration for total button presses
        float stdPressDuration = 0.; // defines the standard deviation duration for all button presses
        float avgReleaseDuration = 0.; // defines the average release duration for total releases
        float stdReleaseDuration = 0.; // defines the standard deviation release duration for total releases
    } buttonConfig;
    
    // Function to handle detecting valid button presses and releases. Also calculates whether or not the button is pressed or released for x amount of time (ms).
    void check_press(int buttonPin) {
        buttonConfig.currentPressTime = millis(); // gets the current time

        // general defining of necessary variables regarding button pressing times and calculations
        if (digitalRead(buttonPin) == HIGH) { // if button is pressed at all
            if (!buttonConfig.isPressed && (buttonConfig.currentPressTime - buttonConfig.lastPressTime > buttonConfig.debounceDelay)) {  // checks that this is the first time the button is pressed
            buttonConfig.isPressed = true; // sets pressed marker to 'true'
            buttonConfig.lastPressTime = buttonConfig.currentPressTime; // update the time of the last press time
            buttonConfig.lastReleaseTime = buttonConfig.currentReleaseTime; // updates time track of last release time after new button press
            } else {
            buttonConfig.isPressed = false; // sets marker to 'false' when button is not pressed OR when debounce is checked
            buttonConfig.currentReleaseTime = millis(); // tracks time the moment of button release
            }
        }

        // Calculates current individual press and release durations. 
        buttonConfig.pressDuration = buttonConfig.currentReleaseTime - buttonConfig.currentPressTime; // calculates duration based on 'current' vars
        buttonConfig.releaseDuration = buttonConfig.currentPressTime - buttonConfig.lastReleaseTime; // calculates release duration based on 'current' press and 'last' release
    };
};

#endif // BUTTON_H