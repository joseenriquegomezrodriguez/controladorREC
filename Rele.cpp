//Rele.cpp
#include "Rele.h"

Rele::Rele(uint8_t Pin,bool Status):pin(Pin),status(Status){

};
void Rele::init(){
   pinMode(pin, OUTPUT);
};
void Rele::setON(){
   status = true;
   digitalWrite(pin, HIGH);
};

void Rele::setOFF(){
   status = false; 
   digitalWrite(pin, LOW);
};

bool Rele::getStatus(){
    return status;
};
