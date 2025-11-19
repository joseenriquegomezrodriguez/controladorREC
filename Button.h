//Button.h
#ifndef Button_h
#define Button_h
#include <Arduino.h>

class Button {
    private:
        uint8_t pin;
    public:
        Button(uint8_t Pin);
        void init();
        int read();
        
};
#endif
