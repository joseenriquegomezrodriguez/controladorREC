//Rele.cpp
#include <Arduino.h>
#include "Rele.h"

Rele::Rele(uint8_t Pin, bool Status) 
    : status(false), _inverted(Status), pin(Pin) { 
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


