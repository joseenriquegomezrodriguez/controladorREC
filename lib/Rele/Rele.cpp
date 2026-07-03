//Rele.cpp
#include <Arduino.h>
#include "Rele.h"

Rele::Rele(uint8_t Pin, bool Status) 
    : status(false), _inverted(Status), pin(Pin),
      _startTime(0), _startPulses(0), _isManualStart(false) { 
};

void Rele::init(){
   pinMode(pin, OUTPUT);
   // Forzar estado inicial apagado físicamente según la lógica de inversión
   status = false;
   digitalWrite(pin, _inverted ? HIGH : LOW); 
};
void Rele::setON(){
   status = true;
   digitalWrite(pin, _inverted ? LOW : HIGH);
};

void Rele::setOFF(){
   status = false; 
   digitalWrite(pin, _inverted ? HIGH : LOW);
};

bool Rele::getStatus(){
    return status;
};

uint32_t Rele::getStartTime() const { return _startTime; }
void Rele::setStartTime(uint32_t time) { _startTime = time; }

uint32_t Rele::getStartPulses() const { return _startPulses; }
void Rele::setStartPulses(uint32_t pulses) { _startPulses = pulses; }

bool Rele::getIsManualStart() const { return _isManualStart; }
void Rele::setIsManualStart(bool manual) { _isManualStart = manual; }
