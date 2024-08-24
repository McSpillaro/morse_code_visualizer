#include <ArduinoSTL.h>
using namespace std;

class MorseCode { // Processes the logic behind the morse code input patterns. Checks for validity of input pattern (i.e. '..-.') as well as calculates short or long presses.
    public: // Allows for objects in class to be used by other project files.
    
    const char* const validPatterns[26] PROGMEM = { // Array containing (already sorted from A-Z) of valid morse code patterns where shortPress=0 and longPress=1.
    "01", "1000", "1010", "100", "0", "0010", "110", "0000", 
    "00", "0111", "101", "0100", "11", "10", "111", "0110", 
    "1101", "010", "000", "1", "001", "0001", "011", "1001", 
    "1011", "1100"
    };

    const char alphabet[27] PROGMEM = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; // Array of 'char' containing a string of the whole alphabet.

    // Initializes the arrays which will handle storing the press and release durations and user input.
    void __init__(int input_size) { 
        char morseCodeInput[input_size] = "";  // Stores the user's combination of short and long presses as a single morse code input.
        int pressDurations[input_size - 1] = {}; // Stores the duration of the user's presses.
        int releaseDurations[input_size - 1] = {}; // Stores the duration of the user's releases.
    };

    // Adds user's input (i.e. "0" or "1" for short or long press) to array containing the pattern. For storing a pattern of inputs, which is later used to handle proper character detection.
    bool addInput(char* array, char newChar, int input_size) { 
        int length = strlen(array); // The current amount of objects in the current array.
        if (length < input_size - 2) { // Ensures there is space for the new character and null terminator
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
    bool addDuration(int* array, int dataPoint, int input_size) {
        int length = sizeof(array) / sizeof(*array); // The current amount of objects in the current array of type 'int'.
        if (length < input_size - 2) { // Ensures there is space for the new data point.
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
    char getLetter(char* user_pattern) {
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

    void clear_input() { // Clears the arrays 'morseCodeInput' and '...Duration' by clearing the memory
        memset(morseCodeInput, 0, sizeof(morseCodeInput)); // clears the user's input in array
        memset(releaseDurations, 0, sizeof(releaseDurations)); // clears the data storing the memory for release durations
        memset(pressDurations, 0, sizeof(pressDurations)); // clears the data storing the memory for press durations
    };
};