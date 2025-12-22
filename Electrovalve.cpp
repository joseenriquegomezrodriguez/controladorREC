// ElectroValvula.cpp
#include "Electrovalve.h"
#include "Program.h"
#include "Rele.h"
#include <RTClib.h>


Electrovalve::Electrovalve(Program* Programs, uint8_t relePin) : Rele(relePin,true){
  
  state = 1;
  
  init();
  

  for (int i = 0; i < 4; ++i) {
        programs[i] = &Programs[i]; 
    }
};

void Electrovalve::changeState(){
  switch (state) {
    case 0: {
        state = 1;
        return;
      }
      case 1: {
        state =2;
        setON();
        return;
      }
      case 2: {
        state = 0;
        setOFF();
        return;
      }
  };
};

char* Electrovalve::getLabelState(){
  // Check bounds just in case, state should be 0, 1, or 2
    if (state < 3) {
        return labelState[state];
    }
    return labelState[0]; // Default to "X" if state is out of bounds
}