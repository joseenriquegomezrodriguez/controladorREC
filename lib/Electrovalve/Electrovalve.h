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
        uint32_t _startTime;
        uint32_t _startPulses;
        const char labelState[3][3]={"X","E","M"};
    public:
        Electrovalve(Program* Programs, uint8_t relePin);
        void check(DateTime date);
        void changeState();
        uint8_t getState() { return state; }
        const char* getLabelState();
        bool isActive() { return getStatus(); }
        void setStartTime(uint32_t t) { _startTime = t; }
        uint32_t getStartTime() { return _startTime; }
        void setStartPulses(uint32_t p) { _startPulses = p; }
        uint32_t getStartPulses() { return _startPulses; }
};
#endif
