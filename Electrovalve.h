//Electrovalve.h
#ifndef Electrovalve_h
#define Electrovalve_h
#include <Arduino.h>
#include "Rele.h"

class Program;

class Electrovalve : public Rele{
    private:
        Program* programs[4];
        uint8_t state;
    public:
        Electrovalve(Program* Programs, uint8_t relePin);
        void check();
        void changeState();
};
#endif
