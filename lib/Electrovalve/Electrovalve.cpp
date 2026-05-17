// ElectroValvula.cpp
#include <Arduino.h>
#include "Electrovalve.h"
#include "Program.h"
#include "Rele.h"
#include <RTClib.h>


Electrovalve::Electrovalve(Program* Programs, uint8_t relePin) : Rele(relePin,true){
  
  state = 1;
  _startTime = 0;

  init();
  

  for (int i = 0; i < 4; ++i) {
        programs[i] = &Programs[i]; 
    }
};

void Electrovalve::changeState(){
  switch (state) {
    case 0: { // En caso de estar en STOP, se cambia a AUTO
        state = 1;
        return;
      }
    case 1: { //En caso de estar en AUTO, se cambia a MANUAL 
        state =2;
        setON();
        return;
      }
      case 2: { //En caso de estar en MANUAL, se cambia a STOP
        state = 0;
        setOFF();
        return;
      }
  };
};

const char* Electrovalve::getLabelState(){
  // Check bounds just in case, state should be 0, 1, or 2
    if (state < 3) {
        return labelState[state];
    }
    return labelState[0]; // Default to "X" if state is out of bounds
}
void Electrovalve::check(DateTime date){
  if (state == 1) { // State AUTO
    bool shouldBeOn = false;
    for (int i = 0; i < 4; ++i) {
      if (programs[i]->inTimeFrame(date)){// Si el programa 'i' está activo, se enciende la válvula
        shouldBeOn = true;
        break;
      }
    }
    if (shouldBeOn) {
        setON(); // Asegura que el relé esté ENCENDIDO
    } else {
        setOFF(); // Asegura que el relé esté APAGADO
    }
  }
  if (state == 2) setON(); // En modo Manual, la válvula debe estar ENCENDIDA
  if (state == 0) setOFF(); // En modo Stop, la válvula debe estar APAGADA
};