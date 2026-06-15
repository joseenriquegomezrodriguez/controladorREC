// ElectroValvula.cpp
#include <Arduino.h>
#include "Electrovalve.h"
#include "Program.h"
#include "Rele.h"
#include <RTClib.h>


Electrovalve::Electrovalve(Program* Programs, uint8_t relePin) : Rele(relePin,true){
  
  state = 1;
  _startTime = 0;
  _startPulses = 0;
  setOFF(); // Asegurar estado inicial cerrado para evitar logs falsos en el primer check

  init();
  

  for (int i = 0; i < 4; ++i) {
        programs[i] = &Programs[i]; 
    }
};

void Electrovalve::changeState(){
  state = (state + 1) % 3; // Ciclo: 0(X) -> 1(E) -> 2(M) -> 0(X)
  
  // Acciones inmediatas al cambiar de modo
  if (state == 2) setON();  // Manual: Abrir ya
  if (state == 0) setOFF(); // Stop: Cerrar ya
};

const char* Electrovalve::getLabelState(){
  // Check bounds just in case, state should be 0, 1, or 2
    if (state < 3) {
        return labelState[state];
    }
    return labelState[0]; // Default to "X" if state is out of bounds
}
void Electrovalve::check(DateTime date, float factor){
  if (state == 1) { // State AUTO
    Program* activeProg = nullptr;
    
    // Buscamos si hay algún programa activo en este momento
    for (int i = 0; i < 4; ++i) {
      if (programs[i]->inTimeFrame(date)){
        activeProg = programs[i];
        break;
      }
    }

    if (activeProg != nullptr) {
        if (!isActive()) {
            // Si la válvula está cerrada y toca regar, la abrimos.
            // El Controller detectará este cambio y fijará _startTime.
            setON(); 
        } else {
            // Si ya está abierta, comprobamos si ha superado la duración ajustada
            uint32_t elapsedSeconds = date.unixtime() - _startTime;
            uint32_t adjustedLimit = (uint32_t)(activeProg->getDuration() * 60.0f * factor);
            
            if (elapsedSeconds >= adjustedLimit) {
                setOFF();
            }
        }
    } else {
        setOFF(); 
    }
  }
  if (state == 2) setON(); // En modo Manual, la válvula debe estar ENCENDIDA
  if (state == 0) setOFF(); // En modo Stop, la válvula debe estar APAGADA
};