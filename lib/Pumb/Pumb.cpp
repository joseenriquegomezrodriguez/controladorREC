//Pumb.cpp
#include <Arduino.h>
#include "Pumb.h"
#include "Electrovalve.h"
#include <RTClib.h>


Pumb::Pumb(uint8_t id, Program** Programs, uint8_t relePin, bool inverted) : Electrovalve(id, Programs, relePin, inverted) {
    // No llamar a init() aquí, dejar que Controller o Main lo hagan
};

void Pumb::changeState() { // Llamar a la función base para cambiar el estado
    DateTime today = clock.now(); // Obtener la fecha y hora actual
    //state = (state + 1) % 3; // Ciclo: 0(X) -> 1(E) -> 2(M) -> 0(X)
   
    if (state == 0) {
        state = 1; // De STOP a AUTO
    } else if (state == 1) {
        state = 2; // De AUTO a MANUAL
    } else if (state == 2) {
        state = 0; // De MANUAL a STOP
    }
     Serial.print(F(">> Cambio de estado de la bomba a: ")); Serial.println(getState());
    switch(getState()) {
        case 0: { // STOP
            
            uint32_t endT = today.unixtime(); // Registrar el tiempo de fin para el log
            uint32_t totalPulses = flowSensor.getPulses() - getStartPulses();
            uint16_t totalLiters = (uint16_t)(totalPulses / (flowSensor.getKFactor() * 60.0));
                
            clock.saveLog( 0x80, totalLiters, getStartTime(), endT);
            clock.addLifetimeLiters(totalLiters);
            
            display.printMode("STOP");

            setIsManualStart(false);
            setStartTime(0);
            setStartPulses(0);
            setValveNoFlowStartTime(0);
            setOFF();
            
            break;
        }
        case 1: // AUTO
            display.printMode("AUTO");
            setOFF(); // En AUTO, la bomba se controla por el programa, inicialmente cerrada
            break;
        case 2: // MANUAL
            display.printMode("MANU");
            setStartTime(today.unixtime()); // Registrar el tiempo de inicio para el log
            setStartPulses(flowSensor.getPulses()); // Registrar los pulsos de
            setIsManualStart(true);
            setON(); // En MANUAL, la bomba debe estar abierta
            break;
    }
    
};
