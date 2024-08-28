#ifndef DURATIONS_H
#define DURATIONS_H

#include <ArduinoSTL.h>

class Durations // Handles structures and functions regarding button presses.
{
    public: // Allows all objects in class to be used by other project files.

    struct ButtonProperties // Structure which defines the key states and properties of the button.
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
    } button_properties;
};

#endif // DURATIONS_H