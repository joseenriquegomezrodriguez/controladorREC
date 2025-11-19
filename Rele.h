//Rele.h
#ifndef Rele_h
#define Rele_h
#include <Arduino.h>
class Rele {
    private:
        bool status;
        uint8_t pin;
    public:
        Rele(uint8_t Pin,bool Status);
        void init();
        void setON();
        void setOFF();
        bool getStatus();
};
#endif
