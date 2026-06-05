//Controler.cpp
//
#include <Arduino.h>
#include "Electrovalve.h"
#include "Controler.h"


Controler::Controler(Electrovalve** v_array, Button** b_array, uint8_t n_valves,  Pumb* PUmb, FlowSensor* flowSensor, Button* modeButton):valves(v_array), valveButtons(b_array), numValves(n_valves), pumb(PUmb), flowSensor(flowSensor), ModeButton(modeButton), keypad(nullptr), useKeypad(false), state(2), backLightDuration(2){
};
Controler::Controler(Electrovalve** v_array, uint8_t n_valves, Pumb* PUmb, FlowSensor* flowSensor, CustomKeypad* keypad) : valves(v_array), valveButtons(nullptr), numValves(n_valves), pumb(PUmb), flowSensor(flowSensor), ModeButton(nullptr), keypad(keypad), useKeypad(true), state(2), backLightDuration(2) {
}

void Controler::setAuto(){
  state = 1;
  display.printMode("AUTO");
};
void Controler::setManual(){
    state = 2;
    pumb->setON();
    display.printMode("MANU");
};
void Controler::setStop(){
    state = 0;
    pumb->setOFF();
    display.printMode("STOP");
};

void Controler::init(){
    if (useKeypad && keypad != nullptr) {
        keypad->init();
    }
};

void Controler::checkBackLight(){
if (display.getBackLight()){
    DateTime today = clock.now();
    int nowMinutes = today.hour() * 60 + today.minute();
    int backLightMinuts = backLightTime.hour() * 60 + backLightTime.minute();
    if (!((nowMinutes >= backLightMinuts) && (nowMinutes < (backLightMinuts + backLightDuration)))){
     display.setOFF();
    };
  };
};

void Controler::changeState(){
  switch (state) {
    case 0: { // De STOP a AUTO
        setAuto();
        return;
      }
      case 1: { // De AUTO a MANUAL
        setManual();
        return;
      }
      case 2: { // De MANUAL a STOP
        setStop();
        return;
      }
  };
};
void Controler::setBackLightTime(DateTime time){
  this->backLightTime = time;
};

void Controler::checkButtons(){
  bool anyButtonPressed = false;
    char printBuffer[20]; // Buffer para sprintf
    static bool lastValveBtnStates[10] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};

    // 1. Recorremos todas las válvulas con un bucle
    for (uint8_t i = 0; i < numValves; i++) {
        bool currentBtnState = valveButtons[i]->read();
        
        // Detectamos el flanco de bajada (de HIGH a LOW) con resistencia Pull-up
        if (currentBtnState == LOW && lastValveBtnStates[i] == HIGH) {
            anyButtonPressed = true;
            delay(50); // Anti-rebote (Debounce) hardware

            // Si la luz está encendida, ejecutamos la acción de la válvula
            if (display.getBackLight()) {
                valves[i]->changeState();
                
                sprintf(printBuffer, "%s%d", valves[i]->getLabelState(), i + 1);

                // ACTUALIZACIÓN DEL DISPLAY
                // Aquí llamamos a una función genérica de tu clase Display
                display.printValveStatus(i, printBuffer); 
            }
        }
        lastValveBtnStates[i] = currentBtnState;
    }

    // 2. Lógica del Backlight (Si se presiona CUALQUIER botón con la luz apagada)
    if (anyButtonPressed && !display.getBackLight()) {
        display.setON();
        setBackLightTime(clock.now());
        
        // Opcional: Pequeño delay para evitar que el primer toque 
        // también active la válvula accidentalmente
        delay(200); 
    }
};

void Controler::checkKeypad() {
    char key = keypad->getKey();
    if (key) {
        // Lógica de Backlight: cualquier tecla lo enciende si está apagado
        if (!display.getBackLight()) {
            display.setON();
            setBackLightTime(clock.now());
            delay(200); // Pequeño delay para evitar ejecución accidental tras despertar
            return; 
        }

        // Cambio de modo de trabajo (usamos '*' como botón de modo)
        if (key == '*') {
            changeState();
            return;
        }

        // Control de electroválvulas (teclas '1' a '9')
        if (key >= '1' && key <= '9') {
            uint8_t i = key - '1';
            if (i < numValves) {
                valves[i]->changeState();
                char printBuffer[20];
                sprintf(printBuffer, "%s%d", valves[i]->getLabelState(), i + 1);
                display.printValveStatus(i, printBuffer);
            }
        }

        // Soporte para válvula 10 con la tecla '0'
        if (key == '0' && numValves >= 10) {
            valves[9]->changeState();
            char printBuffer[20];
            sprintf(printBuffer, "%s%d", valves[9]->getLabelState(), 10);
            display.printValveStatus(9, printBuffer);
        }
    }
}

void Controler::checkIrrigation(DateTime today){
  char printBuffer[20]; // Buffer para sprintf
  static uint32_t lastDisplayUpdate = 0;
  bool anyActive = false; 
  uint32_t now = millis();
  
  // Determine blink state (toggle every 500ms)
  bool blinkVisible = (now / 500) % 2;

  // Actualizar caudal una sola vez por ciclo de refresco
  if (now - lastDisplayUpdate >= 200) {
      display.printFlow(flowSensor->getInstantFlow());
  }
  
  // 1. Recorremos todas las válvulas con un bucle
    for (uint8_t i = 0; i < numValves; i++) {
      bool wasActive = valves[i]->isActive();
      
      // Si el controlador está en STOP (0), forzamos el cierre de la válvula
      if (state == 0) {
          valves[i]->setOFF();
      } else {
          valves[i]->check(today);
      }

      bool isNowActive = valves[i]->isActive();

      // Detectar Inicio de Riego
      if (!wasActive && isNowActive) {
          valves[i]->setStartTime(today.unixtime());
          flowSensor->reset();
          valves[i]->setStartPulses(flowSensor->getPulses());
      } 
      // Detectar Fin de Riego
      else if (wasActive && !isNowActive) {
          uint32_t startT = valves[i]->getStartTime();
          uint32_t endT = today.unixtime();
          
          // Calculamos la diferencia de pulsos y convertimos a litros
          uint32_t totalPulses = flowSensor->getPulses() - valves[i]->getStartPulses();
          // Usamos la misma fórmula que FlowSensor::getVolume() pero sobre el delta
          uint16_t totalLiters = (uint16_t)(totalPulses / (7.5 * 60.0)); 
          clock.saveLog(i + 1, totalLiters, startT, endT);
      }
      
      if (isNowActive) anyActive = true;
      
      if (now - lastDisplayUpdate >= 200) {
          // Implement blink: if valve is active, toggle visibility
          if (valves[i]->isActive() && !blinkVisible) {
              sprintf(printBuffer, "  "); // Borra solo la posición de esa válvula
          } else {
              sprintf(printBuffer, "%s%d", valves[i]->getLabelState(), i + 1);
          }
          display.printValveStatus(i, printBuffer);
      }
    }

    // Control maestro de la bomba respetando el estado del controlador
    if (state == 0) {
        pumb->setOFF(); // STOP: Bomba siempre apagada
    } else if (state == 2) {
        pumb->setON();  // MANUAL: Bomba siempre encendida (según tu setManual)
    } else {
        // AUTO: La bomba depende de si hay válvulas abiertas
        if (anyActive) pumb->setON(); else pumb->setOFF();
    }

    if (now - lastDisplayUpdate >= 200) lastDisplayUpdate = now;
};
void Controler::check(){
  DateTime today = clock.now();
  uint32_t now = millis();
  static uint32_t lastSlowUpdate = 0;

  // Permite ajustar la hora via Serial si es necesario
  clock.syncWithSerial();

  // Actualizar información estática/lenta solo cada 1 segundo para evitar saturar el I2C
  if (now - lastSlowUpdate >= 1000) {
      display.printHour(clock.getHour());
      display.printDay(clock.getDayOfTheWeek());
      display.printStation(clock.getStacion());
      lastSlowUpdate = now;
  }
  
  if (useKeypad) {
      checkKeypad();
  } else {
     checkButtons();
      // Lógica para el botón de modo físico con detección de flanco e inversión
      if ((ModeButton != nullptr)&&(display.getBackLight())) {
          static bool lastModeBtnState = HIGH;
          bool currentModeState = ModeButton->read();
          if (currentModeState == LOW && lastModeBtnState == HIGH) changeState();
          lastModeBtnState = currentModeState;
      }
      if ((ModeButton != nullptr) && !display.getBackLight()) {
        display.setON();
        setBackLightTime(clock.now());
        
        // Opcional: Pequeño delay para evitar que el primer toque 
        // también active la válvula accidentalmente
        delay(200); 
    }

  }
  this->checkBackLight();
  this->checkIrrigation(today);
};
