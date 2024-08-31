#ifndef CALCULATE_HPP
#define CALCULATE_HPP

#include <ArduinoSTL.h>

class Calculate
{
public: // Allows all objects in class to be used by other project files.
    // Calculates the average of the values (int*) in the given array and returns a (float).
    float averageArray(int *array)
    {
        int sum = 0;                                    // Sum of the data points in the given array.
        int arraySize = sizeof(array) / sizeof(*array); // Current length of the array.
        for (int i = 0; i < arraySize; i++)
        {                    // Loops through the array.
            sum += array[i]; // Calculates the sum from each array datapoint.
        }
        float avgResult = sum / sizeof(array); // Calculates the average of array.
        return avgResult;
    };

    // Calculates the standard deviation of the given array (int*) and given average of array (float) and returns (float).
    float stdArray(int *array, float avg)
    {
        int arraySize = sizeof(array) / sizeof(*array); // Current length of the array.
        float sum = 0.;                                 // Sum of: (datapoints - average of array)^2
        float stdResult = 0;                            // Result of the calculated standard deviation.
        for (int i = 0; i < arraySize; i++)
        {                                               // Loops through the current size of array.
            sum += (array[i] - avg) * (array[i] - avg); // Calculates the numerator in std formula.
        }
        stdResult = sqrt(sum / (arraySize - 1)); // Calculates standard deviation.
        return stdResult;
    };
};

#endif // CALCULATE_HPP