#ifndef MORSE_CODE_H
#define MORSE_CODE_H

#include <ArduinoSTL.h>
#include "configuration.h"
#include "calculate.h"

class MorseCode { // Processes the logic behind the morse code input patterns. Checks for validity of input pattern (i.e. '..-.') as well as calculates short or long presses.
    private: // Constants accessed by only this class are private. 
    Configuration::ButtonProperties button_properties; // Duration, state, and defintion properties of a button.
    Configuration::InputArray input_array; // Arrays which store user input as well the durations of presses and releases from the user.
    Calculate calculator; // Instance of 'Calculate' class which is used to call functions from class.

    const char* const validPatterns[26] PROGMEM = { // Array containing (already sorted from A-Z) of valid morse code patterns where shortPress=0 and longPress=1.
    "01", "1000", "1010", "100", "0", "0010", "110", "0000", 
    "00", "0111", "101", "0100", "11", "10", "111", "0110", 
    "1101", "010", "000", "1", "001", "0001", "011", "1001", 
    "1011", "1100"
    };

    const char alphabet[27] PROGMEM = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; // Array of 'char' containing a string of the whole alphabet.


    public: // Allows for objects in class to be used by other project files.
   
    // Adds user's input (i.e. "0" or "1" for short or long press) to array containing the pattern. For storing a pattern of inputs, which is later used to handle proper character detection.
    bool add_input(char* array, char newChar, int inputSize) { 
        int length = strlen(array); // The current amount of objects in the current array.
        if (length < inputSize - 2) { // Ensures there is space for the new character and null terminator
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
    bool add_duration(int* array, int dataPoint, int inputSize) {
        int length = sizeof(array) / sizeof(*array); // The current amount of objects in the current array of type 'int'.
        if (length < inputSize - 2) { // Ensures there is space for the new data point.
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

    // Proccesses and handles the detection of user inputs whether or not they are short ('0') or long ('1') presses. Returns a boolean of 'true' (if successful input) or 'false' (if not) and adds that to user input array of morse code pattern.
    boolean check_input() {
        button_properties.avgPressDuration = calculator.averageArray(input_array.pressDurations); // Calculates the average of press durations
        button_properties.avgReleaseDuration = calculator.averageArray(input_array.releaseDurations); // Calculates the average of release durations
        button_properties.stdPressDuration = calculator.stdArray(input_array.pressDurations, button_properties.avgPressDuration); // Calculates standard deviation
        button_properties.stdReleaseDuration = calculator.stdArray(input_array.releaseDurations, button_properties.avgReleaseDuration); // Calculates standard deviation

        float threshold = 1.5; // Threshold for learned short and long press/release durations.
        
    }
};

#endif // MORSE_CODE_H