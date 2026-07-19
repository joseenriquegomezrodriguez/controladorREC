//Electrovalve.h
#ifndef Electrovalve_h
#define Electrovalve_h
#include <Arduino.h>
#include "Rele.h"
#include <RTClib.h>
#include "Clock.h"
#include "Display.h"
#include "FlowSensor.h"

class Program;

extern Display display;
extern Clock clock;
extern FlowSensor flowSensor;

class Electrovalve : public Rele{
    private:
        Program** programs;
        uint8_t id;
        const char labelState[3][3]={"X","E","M"};
        uint8_t lastValveStates;
        uint32_t valveNoFlowStartTime;
        uint32_t leakStartTime; // Time when leak was detected
        uint32_t leakStartPulses; // Pulses when leak was detected
       
        uint32_t _startTime;
        uint32_t _startPulses;
        bool _isManualStart;
          
    protected:
        uint8_t state;

    public:
        Electrovalve(uint8_t id, Program** Programs, uint8_t relePin, bool inverted = false);
        void check(DateTime date, float factor);
        void changeState();
        uint8_t getState() { return state; }
        void setState(uint8_t newState) { state = newState; }
        const char* getLabelState();
        //bool isActive() { return getStatus(); }
        uint32_t getValveNoFlowStartTime() { return valveNoFlowStartTime; }
        void setValveNoFlowStartTime(uint32_t time) { valveNoFlowStartTime = time; }
        uint8_t getLastValveStates() { return lastValveStates; }
        void setLastValveStates(uint8_t state) { lastValveStates = state; }
        uint8_t getId() { return id; }
        uint32_t getLeakStartTime() { return leakStartTime; }
        void setLeakStartTime(uint32_t time) { leakStartTime = time; }
        uint32_t getLeakStartPulses() { return leakStartPulses; }
        void setLeakStartPulses(uint32_t pulses) { leakStartPulses = pulses; }
        
        uint32_t getStartTime() const;
        void setStartTime(uint32_t time);
        uint32_t getStartPulses() const;
        void setStartPulses(uint32_t pulses);
        bool getIsManualStart() const;
        void setIsManualStart(bool manual);
};

#endif
