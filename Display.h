//Display.h
#ifndef Display_h
#define Display_h
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class Display {
    private:
        LiquidCrystal_I2C lcd;
        bool backLight;
       
    public:
        Display();
        void init();
        void setON();
        void setOFF();
        bool getBackLight();
        void print(uint8_t X, uint8_t Y, char16_t txt);
};
#endif
