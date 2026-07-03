//Electrovalve.h
#ifndef Electrovalve_h
#define Electrovalve_h
#include <Arduino.h>
#include "Rele.h"
#include <RTClib.h>
class Program;

class Electrovalve : public Rele{
    private:
        Program* programs[4];
        uint8_t state;
        const char labelState[3][3]={"X","E","M"};
    public:
        Electrovalve(Program* Programs, uint8_t relePin, bool inverted = false);
        void check(DateTime date, float factor);
        void changeState();
        uint8_t getState() { return state; }
        const char* getLabelState();
        bool isActive() { return getStatus(); }
};
#endif
