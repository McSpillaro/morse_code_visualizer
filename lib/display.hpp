#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <Arduino.h>

class Display
{
private:
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
                if (i + 1 == MAX_LCD_SLOTS)
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

#endif // DISPLAY_HPP