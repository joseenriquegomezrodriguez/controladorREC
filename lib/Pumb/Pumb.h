//Pumb.h
#ifndef Pumb_h
#define Pumb_h
#include <Arduino.h>
#include "Electrovalve.h"

class Pumb : public Electrovalve{
    private:
        //const char labelState[3][5]={"STOP","AUTO","MANU"};
    public:
        Pumb(uint8_t id, Program** Programs, uint8_t relePin, bool inverted = false);
        
        void changeState();
};
#endif
