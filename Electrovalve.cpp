// ElectroValvula.cpp
#include "Electrovalve.h"
#include "Program.h"
#include "Rele.h"
#include <RTClib.h>


Electrovalve::Electrovalve(Program* Programs, uint8_t relePin) : Rele(relePin,true){
  
  state = 1;
  
  init();
  setOFF();

  for (int i = 0; i < 4; ++i) {
        programs[i] = &Programs[i]; 
    }
};

void Electrovalve::changeState(){

};