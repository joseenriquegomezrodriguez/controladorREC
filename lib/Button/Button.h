//Button.h
#ifndef Button_h
#define Button_h
#include <Arduino.h>

class Button {
    private:
        bool lastState;
        uint8_t pin;
    public:
        Button(uint8_t Pin);
        void init();
        int read();
        void setLastState(bool state);
        bool getLastState();
        
};
#endif
