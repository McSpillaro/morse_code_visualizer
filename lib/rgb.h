class Light {
    public: // Allows for objects in class to be used by other project files.

    // Sets the color of the RGB light where rgb(LOW-HIGH, LOW-HIGH, LOW-HIGH)
    void set_color(uint8_t R, uint8_t G, uint8_t B) { 
    digitalWrite(pin.r, R);
    digitalWrite(pin.b, B);
    digitalWrite(pin.g, G);
    };
};