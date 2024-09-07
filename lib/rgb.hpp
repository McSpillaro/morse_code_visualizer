#ifndef RGB_HPP
#define RGB_HPP

// Handles the changing of color as well as turning on/off the rgb light.
class Light
{
public:
    // Defining the variables for the digital pin I/O on RGB.
    const int r = 10, g = 9, b = 6;

    // Turns off the RGB light indicator.
    void off()
    {
        digitalWrite(r, LOW);
        digitalWrite(g, LOW);
        digitalWrite(b, LOW);
    };

    // Sets color of light indicator.
    void color(uint8_t R, uint8_t G, uint8_t B)
    {
        digitalWrite(r, R);
        digitalWrite(g, G);
        digitalWrite(b, B);

        // Serial output
        Serial.println("Indiactor successfully changed");
    };
};

#endif // RGB_HPP