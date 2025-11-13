//Rele.cpp
#include "Rele.h"

Rele::Rele(uint8_t Pin,bool Status):pin(Pin),status(Status){


};

void Rele::setON(){
   status = true; 
};

void Rele::setOFF(){
   status = false; 
};

bool Rele::getStatus(){
    return status;
};