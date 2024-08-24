class Light {
    public: // Allows for objects in class to be used by other project files.

    // Turns off the RGB light indicator.
    void off() {
        digitalWrite(pin.r, LOW);
        digitalWrite(pin.g, LOW);
        digitalWrite(pin.b, LOW);
    };

    // Sets color of light indicator.
    void color(uint8_t R, uint8_t G, uint8_t B) {
        digitalWrite(pin.r, R);
        digitalWrite(pin.g, G);
        digitalWrite(pin.b, B);
    };
};