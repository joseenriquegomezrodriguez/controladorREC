// ElectroValvula.cpp
#include <Arduino.h>
#include "Electrovalve.h"
#include "Program.h"
#include "Rele.h"
#include <RTClib.h>


Electrovalve::Electrovalve(uint8_t id, Program** Programs, uint8_t relePin, bool inverted) 
: Rele(relePin, inverted), programs(Programs), id(id), _startTime(0), _startPulses(0), _isManualStart(false) {
  
  state = 0;
  setOFF(); // Asegurar estado inicial cerrado para evitar logs falsos en el primer check
};

uint32_t Electrovalve::getStartTime() const { return _startTime; }
void Electrovalve::setStartTime(uint32_t time) { _startTime = time; }

uint32_t Electrovalve::getStartPulses() const { return _startPulses; }
void Electrovalve::setStartPulses(uint32_t pulses) { _startPulses = pulses; }

bool Electrovalve::getIsManualStart() const { return _isManualStart; }
void Electrovalve::setIsManualStart(bool manual) { _isManualStart = manual; }

void Electrovalve::changeState(){
  DateTime today = clock.now();
  state = (state + 1) % 3; // Ciclo: 0(X) -> 1(E) -> 2(M) -> 0(X)
  //hacer las acciones necesarias según el nuevo estado
  if (state == 0) { // STOP
    setOFF();
  } else if (state == 1) { // AUTO
    setOFF(); // En AUTO, la válvula se controla por el programa, inicialmente cerrada
  } else if (state == 2) { // MANUAL
    setON(); // En MANUAL, la válvula debe estar abierta
    setIsManualStart(true);
    setStartTime(today.unixtime()); // Registrar el tiempo de inicio para el log
    setStartPulses(flowSensor.getPulses()); // Registrar los pulsos iniciales
  }
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
            // BLOQUEO DE REINICIO: Solo permite abrir la válvula en el minuto 00.
            // Si el riego termina (ej. a los 9 min por factor), al ser ya el minuto 09,
            // esta condición impedirá que se vuelva a abrir inmediatamente.
            if (date.minute() == 0) {
                setON(); 
            }
        } else {
            // Si ya está abierta, comprobamos si ha superado la duración ajustada
            uint32_t elapsedSeconds = date.unixtime() - _startTime;
            uint32_t adjustedLimit = (uint32_t)(activeProg->getDuration() * 60.0f * factor);
            
            if (elapsedSeconds >= adjustedLimit) {
                setOFF();
            }
        }
    } else {
        //comprobar que al no haber programa activo, la válvula esté cerrada.
        if (isActive()) {
            setOFF();
        }

    }
  }
  if (state == 2){ // State Electorvalve MANUAL
    // si no hay fujo, comprobar que no haya pasado más de 5 segundos desde que se abrió la válvula y si es así, cerrar la válvula y registrar el log de riego manual.
  
    // Aquí podrías agregar lógica para detectar flujo o falta de flujo y registrar logs si es necesario
  }  
  if (state == 0){ // State Electorvalve STOP
    //comprobar posibles fugas.
  } 
};