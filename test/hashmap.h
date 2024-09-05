#include <ArduinoSTL.h>

const int TABLE_SIZE = 26; // Size of the hash table, one slot for each letter in the alphabet

// Hashmap entry (key-value pair)
struct Entry {
    char key;            // The key (a single character)
    const char* value;   // The value (a Morse code pattern)
    bool isOccupied;     // A flag to check if the slot is occupied
};

// Simple hash function to compute an index based on the key (character)
int hashFunction(char key) {
    return (key - 'A') % TABLE_SIZE;  // Calculate the index based on the alphabetical position
}

// Define a HashMap class to manage the hash table
class HashMap {
private:
    Entry table[TABLE_SIZE];  // Fixed-size array of Entry structs, representing the hash table

public:
    // Constructor to initialize the hash table with predefined key-value pairs
    HashMap(const char* const patterns[], const char* alphabet) {
        // Initialize all slots as unoccupied
        for (int i = 0; i < TABLE_SIZE; i++) {
            table[i].isOccupied = false;
        }

        // Insert predefined key-value pairs (letters and their Morse code patterns)
        for (int i = 0; i < TABLE_SIZE; i++) {
            put(alphabet[i], patterns[i]);
        }
    }

    // Method to insert a key-value pair into the hash table
    void put(char key, const char* value) {
        int index = hashFunction(key);  // Compute the index using the hash function

        // Handle collisions using linear probing (although in this case, we expect no collisions)
        while (table[index].isOccupied && table[index].key != key) {
            index = (index + 1) % TABLE_SIZE;  // Move to the next index (circularly)
        }

        // Insert the key-value pair into the table
        table[index].key = key;          // Set the key
        table[index].value = value;      // Set the value
        table[index].isOccupied = true;  // Mark the slot as occupied
    }

    // Method to retrieve the value (Morse code pattern) associated with a given key (character)
    const char* get(char key) const {
        int index = hashFunction(key);  // Compute the index using the hash function

        // Search for the key using linear probing
        while (table[index].isOccupied) {  // Continue searching while the slot is occupied
            if (table[index].key == key) {  // If the key matches
                return table[index].value;  // Return the associated value (Morse code pattern)
            }
            index = (index + 1) % TABLE_SIZE;  // Move to the next index (circularly)
        }

        return nullptr;  // Return nullptr if the key is not found
    }
};

int main() {
    // Define the valid Morse code patterns and corresponding alphabet
    const char* const validPatterns[26] = {
        "01", "1000", "1010", "100", "0", "0010", "110", "0000",
        "00", "0111", "101", "0100", "11", "10", "111", "0110",
        "1101", "010", "000", "1", "001", "0001", "011", "1001",
        "1011", "1100"};

    const char alphabet[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    // Create an instance of HashMap with predefined key-value pairs
    HashMap map(validPatterns, alphabet);

    // Test retrieving Morse code patterns for specific letters
    std::cout << "Morse code for 'A': " << map.get('A') << std::endl;
    std::cout << "Morse code for 'B': " << map.get('B') << std::endl;
    std::cout << "Morse code for 'Z': " << map.get('Z') << std::endl;

    return 0;
}