//Rele.h
#ifndef Rele_h
#define Rele_h
#include <Arduino.h>
class Rele {
    protected:
        bool status;
        bool _inverted;
        uint8_t pin;

        

    public:
        Rele(uint8_t Pin,bool Status);
        virtual void init();
        virtual void setON();
        virtual void setOFF();
        bool getStatus(); // Mantenido por compatibilidad
        bool isActive() const { return status; }

        
};
#endif
