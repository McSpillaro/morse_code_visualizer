#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <Arduino.h>

class Properties
{
public:
    struct ButtonProperties // Structure which defines the key states and properties of the button.
    {
        /*
        Important moments of button functionality that needs to be tracked:
        * initial button press
        * initial button release
        * next button press
        * next button release
        */

        bool isPressed = false;                          // boolean value for whether or not the button is pressed
        unsigned long currentPressTime = 0;              // to track the time of the current button press
        unsigned long currentReleaseTime = 0;            // to track the time of the button the moment it is released
        unsigned long lastPressTime = 0;                 // to track the time of the last button press
        unsigned long lastReleaseTime = 0;               // tracks last time the button was released
        unsigned long pressDuration = 0;                 // stores the duration of the button press
        unsigned long releaseDuration = 0;               // stores duration of button not being pressed
        const unsigned long debounceDelay = 50;          // debounce time in milliseconds
        const unsigned long clearScreenThreshold = 2000; // hold button for 2 seconds to clear lcd
        const unsigned long inputFinalThreshold = 2000;  // release button for 2 seconds to finalize input
        const int shortPressCap = 100;                   // 100 ms hardcap to detect a short press
        const int longPressCap = 300;                    // 300 ms hardcap to detect a long press

        bool inputFinal = false;      // Whether or not the user is done inputting presses for morse code
        bool inputClearFinal = false; // Whether or not the user wants to clear the screen or not

        char shortPress = '0'; // defines the short press as char 0
        char longPress = '1';  // defines the long press as char 1

        float avgPressDuration = 0.;   // defines the average press duration for total button presses
        float stdPressDuration = 0.;   // defines the standard deviation duration for all button presses
        float avgReleaseDuration = 0.; // defines the average release duration for total releases
        float stdReleaseDuration = 0.; // defines the standard deviation release duration for total releases
    } button_properties;
};

class Button
{ // Handles structures and functions regarding button presses.

private:
    Properties::ButtonProperties button_properties; // Duration, state, and defintion properties of a button.

public: // Allows all objects in class to be used by other project files.
    // Boolean function to check whether or not the correct

    // Function to handle detecting valid button presses and releases. Also calculates whether or not the button is pressed or released for x amount of time (ms). Essentially just adds data durations.
    void calc_press(int buttonPin)
    {
        button_properties.currentPressTime = millis(); // gets the current time

        // general defining of necessary variables regarding button pressing times and calculations
        if (digitalRead(buttonPin) == HIGH) // if button is pressed at all
        {
            if (!button_properties.isPressed && (button_properties.currentPressTime - button_properties.lastPressTime > button_properties.debounceDelay)) // checks that this is the first time the button is pressed
            {
                button_properties.isPressed = true;                                       // sets pressed marker to 'true'
                button_properties.lastPressTime = button_properties.currentPressTime;     // update the time of the last press time
                button_properties.lastReleaseTime = button_properties.currentReleaseTime; // updates time track of last release time after new button press
            }
            else
            {
                button_properties.isPressed = false;             // sets marker to 'false' when button is not pressed OR when debounce is checked
                button_properties.currentReleaseTime = millis(); // tracks time the moment of button release
            }
        }

        // Calculates current individual press and release durations.
        button_properties.pressDuration = button_properties.currentReleaseTime - button_properties.currentPressTime; // calculates duration based on 'current' vars
        button_properties.releaseDuration = button_properties.currentPressTime - button_properties.lastReleaseTime;  // calculates release duration based on 'current' press and 'last' release
    };
};

#endif // BUTTON_HPP