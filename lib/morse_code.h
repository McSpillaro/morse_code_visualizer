#ifndef MORSE_CODE_H
#define MORSE_CODE_H

#include <ArduinoSTL.h>
#include <functional>
#include "button.h"
#include "calculate.h"

class Data { // Holds the arrays specific to storing user input and processing it for morse code detection.   
    public:
        static const int MAX_INPUT_SIZE = 5; // Max input size for morse code (4), which includes the buffer (1)
        
        // Contains the arrays which stores user input as well as duration array of short and long presses.
        struct InputArray {
            char userInput[MAX_INPUT_SIZE] = "";                         // Stores the user's combination of short and long presses as a single morse code input.
            int pressDurations[MAX_INPUT_SIZE - 1] = {};   // Stores the duration of the user's presses.
            int releaseDurations[MAX_INPUT_SIZE - 1] = {}; // Stores the duration of the user's releases.
        } input_array;
};

class MorseCode { // Processes the logic behind the morse code input patterns. Checks for validity of input pattern (i.e. '..-.') as well as calculates short or long presses.
    private:
        Properties::ButtonProperties button_properties; // Duration, state, and defintion properties of a button.
        Data::InputArray input_array;                   // Arrays which store user input as well the durations of presses and releases from the user.
        Calculate calculator;                           // Instance of 'Calculate' class which is used to call functions from class.

        const char *const validPatterns[26] PROGMEM = { // Array containing (already sorted from A-Z) of valid morse code patterns where shortPress=0 and longPress=1.
            "01", "1000", "1010", "100", "0", "0010", "110", "0000",
            "00", "0111", "101", "0100", "11", "10", "111", "0110",
            "1101", "010", "000", "1", "001", "0001", "011", "1001",
            "1011", "1100"};

        const char alphabet[27] PROGMEM = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; // Array of 'char' containing a string of the whole alphabet.

        // Function to handle short presses
        void handleShortPress() {
            // Adds char '0' to char* userInput
            add_input(input_array.userInput, button_properties.shortPress);
        };

        // Function to handle long presses
        void handleLongPress() {
            // Adds char '1' to char* userInput
            add_input(input_array.userInput, button_properties.longPress);
        };

        // Function to handle clearing the lcd
        void handleHoldPress() {
            // Clears the lcd screen

        };

    public: 

        // Adds user's input (i.e. "0" or "1" for short or long press) to array containing the pattern. For storing a pattern of inputs, which is later used to handle proper character detection.
        bool add_input(char* array, char newChar) { 
            int length = strlen(array); // The current amount of objects in the current array.
            if (length < Data::MAX_INPUT_SIZE - 2) { // Ensures there is space for the new character and null terminator
                array[length] = newChar; // Adds to the end of the array the user's input.
                array[length + 1] = '\0'; // Null-terminate the string.
                // Serial output
                Serial.print("Added to pattern: ");
                Serial.println(newChar);
                return true; // Successfully added
            } else {
                Serial.println("Array containing pattern is full.");
                return false; // Array is full
            }
        };

        // Adds press or release durations to the respective array for handling short or long press variability detection.
        bool add_duration(int* array, int dataPoint) {
            int length = sizeof(array) / sizeof(*array); // The current amount of objects in the current array of type 'int'.
            if (length < Data::MAX_INPUT_SIZE - 2) { // Ensures there is space for the new data point.
                array[length] = dataPoint; // Adds the data point to the array.
                // Serial output
                Serial.print("Added to duration array: ");
                Serial.println(dataPoint);
                return true; // Successfully added data point
            } else {
                Serial.println("Array containing data points is full.");
                return false; // Array is full
            }
        };

        // Gets the corresponding letter from the user's input pattern, if valid, from program memory.
        char get_letter(char* user_pattern) {
            for (int i = 0; i < 26; i++) { // Loops through alphabet[] array size
                if (strcmp_P(user_pattern, (char*)pgm_read_word(&(validPatterns[i]))) == 0) { // Compares RAM-based 'code' string to 'alphabet' flash memory string; returns 0 if a match.
                    // Serial output
                    Serial.print("Morse code ");
                    Serial.print(user_pattern);
                    Serial.print(" matches letter ");
                    Serial.println((char)pgm_read_byte(&(alphabet[i])));
                    return pgm_read_byte(&(alphabet[i])); // Address of the morse code string for the letter at index 'i' in the 'morseCode' memory.
                }
            }

            // If the user's input pattern is invalid (there's no match)
            Serial.print("Morse code ");
            Serial.print(user_pattern);
            Serial.println(" does not match any letter.");
            return '?'; // Return '?' if no match found
        };
        
        // Clears user's input morse code pattern which is stored in memory.
        void clear_input(char* userInput) { 
            memset(userInput, 0, sizeof(userInput));
        };

        // Clears the arrays storing the duration of presses and releases for calculating avg and std.
        void clear_duration(int* array) {
            memset(array, 0, sizeof(array));
        };

        // Proccesses and handles the detection of user inputs whether or not they are short ('0') or long ('1') presses. Returns int '0' (if successful input) or '9' (if not) and adds that to user input array of morse code pattern.
        int check_press() {
            int errorCode = 0;

            button_properties.avgPressDuration = calculator.averageArray(input_array.pressDurations);     // Calculates the average of press durations
            button_properties.avgReleaseDuration = calculator.averageArray(input_array.releaseDurations); // Calculates the average of release durations
            button_properties.stdPressDuration = calculator.stdArray(input_array.pressDurations, button_properties.avgPressDuration); // Calculates standard deviation
            button_properties.stdReleaseDuration = calculator.stdArray(input_array.releaseDurations, button_properties.avgReleaseDuration); // Calculates standard deviation

            float threshold = 1.5; // Threshold for learned short and long press/release durations.

            // Function pointer to dynamically handle short or long presses
            void (MorseCode::*pressHandler)() = nullptr;

            // Determines whether the current press is short or long
            if (button_properties.pressDuration <= button_properties.avgPressDuration + threshold * button_properties.stdPressDuration || button_properties.pressDuration <= button_properties.shortPressCap) {
                pressHandler = &MorseCode::handleShortPress;
            } else if (button_properties.pressDuration >= button_properties.avgPressDuration + threshold * button_properties.stdPressDuration || button_properties.shortPressCap <= button_properties.pressDuration <= button_properties.longPressCap) {
                pressHandler = &MorseCode::handleLongPress;
            };

            // Execute the appropriate handler
            if (pressHandler) {
                (this->*pressHandler)();
                return errorCode; // Successfully processed user input; returns 0
            }

            return errorCode = 9; // Did not processed user input; returns 9
        };

        // Processes the user's morse code pattern if it is valid compared to that defined in the array. Returns 'true' if successful.
        boolean check_pattern() {
            // Fetches a char (A-Z or ?) based on the user's inputted pattern.
            char patternResult = get_letter(input_array.userInput);
            
            if (patterResult == '?') { // Returns boolean value if the users input pattern was a successful char or '?' (error)
                return false; // Cannot find a valid letter based on users pattern
            };

            return true; // Successfully found letter based on users pattern
        };

#endif // MORSE_CODE_H