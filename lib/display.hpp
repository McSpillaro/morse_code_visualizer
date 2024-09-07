#ifndef LCD_HPP
#define LCD_HPP

#include <Arduino.h>
#include <LiquidCrystal.h>

class ConfigLCD
{
public:
    // Specific for configuaring the lcd and its corresponding rows by an array for manual changing of slots
    struct LcdConfiguration // Sets up the LCD to have predefined lines for buffering
    {
        static const int MAX_LCD_SLOTS = 17; // Max amount of lcd columns in one row.

        static const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2; // Defining the variables for the digital pin I/O on LCD and RGB light.

        char line0[MAX_LCD_SLOTS] = ""; // Array containing characters which will be displayed on the first row of the lcd.
        char line1[MAX_LCD_SLOTS] = ""; // Array containing characters which will be displayed on the second row of the lcd.
    };
};

class Display
{
private:
    ConfigLCD::LcdConfiguration lcd_config; // Instance of 'ConfigLCD' class with struct containing pins and arrays.

    void lcd_scroll(char letter)
    {
        int length0 = strlen(lcd_config.line0); // length of the array for the top row on the lcd
        int length1 = strlen(lcd_config.line1); // length of the array for the bottom row on the lcd

        if (length0 == 0)
        {                                 // if the lcd is completely fresh/blank; runs here and stops
            lcd_config.line0[0] = letter; // updates the 1st column, 1st row slot with the letter
        }
        else
        {
            for (int i = 0; i <= length0; i++)
            {                                                  // goes through each filled slot in the 1st lcd row
                lcd_config.line0[i + 1] = lcd_config.line0[i]; // sets next slot to be the next to be the prev character
                if (i + 1 == lcd_config.MAX_LCD_SLOTS)
                {                                              // if the 1st row of the lcd is full
                    lcd_config.line1[0] = lcd_config.line0[i]; // moves last character in 1st row to start of 2nd row
                    for (int j = 0; j < length1; j++)
                    {                                                  // goes through each filled slot in the 2nd lcd row
                        lcd_config.line1[j + 1] = lcd_config.line1[j]; // sets next slot to be the prev character
                    }
                }
            }
            lcd_config.line0[0] = letter; // updates (0,0);(c,r) with the letter after scrolling update
        }
    };

public:
    // Updates the display of the LCD including the buffer
    void update_display(char letter)
    {
        lcd_scroll(letter);
        lcd.setCursor(0, 0);         // Sets cursor to the upper left corner (1st column, 1st row)
        lcd.print(lcd_config.line0); // Prints to the top row
        lcd.setCursor(0, 1);         // Sets cursor to the bottom left corner (1st column, 2nd row)
        lcd.print(lcd_config.line1); // Prints to the bottom row
    };
};

#endif // LCD_HPP