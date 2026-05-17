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
  state = 0;
  display.printMode("AUTO");
};
void Controler::setManual(){
    state = 1;
    pumb->setON();
    display.printMode("MANU");
};
void Controler::setStop(){
    state = 2;
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
    case 0: {
        setManual();
        return;
      }
      case 1: {
        setStop();
        return;
      }
      case 2: {
        setAuto();
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

    // 1. Recorremos todas las válvulas con un bucle
    for (uint8_t i = 0; i < numValves; i++) {
        
        // Comprobamos si el botón de la válvula 'i' está presionado
        if (valveButtons[i]->read() == LOW) {
            anyButtonPressed = true;

            // Si la luz está encendida, ejecutamos la acción de la válvula
            if (display.getBackLight()) {
                valves[i]->changeState();

                // Armamos el texto (ej: "ON 1", "OFF 2", etc.)
                // Usamos i + 1 para que el usuario vea "Válvula 1" en vez de "Válvula 0"
                sprintf(printBuffer, "%s%d", valves[i]->getLabelState(), i + 1);

                // ACTUALIZACIÓN DEL DISPLAY
                // Aquí llamamos a una función genérica de tu clase Display
                display.printValveStatus(i, printBuffer); 
            }
        }
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
  uint32_t now = millis();
  
  // Determine blink state (toggle every 500ms)
  bool blinkVisible = (now / 500) % 2;
  
  // 1. Recorremos todas las válvulas con un bucle
    for (uint8_t i = 0; i < numValves; i++) {
      bool wasActive = valves[i]->isActive();
      
      valves[i]->check(today);

      bool isNowActive = valves[i]->isActive();

      // Detectar Inicio de Riego
      if (!wasActive && isNowActive) {
          valves[i]->setStartTime(today.unixtime());
          pumb->setON();
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
          pumb->setOFF();
          clock.saveLog(i + 1, totalLiters, startT, endT);
      }
      
      // Only update the physical display every 200ms to save CPU and I2C bandwidth
      if (now - lastDisplayUpdate >= 200) {
          const char* stateLabel = valves[i]->getLabelState();
          
          // Implement blink: if state is not STOP ('X'), toggle visibility
          if (stateLabel[0] != 'X' && !blinkVisible) {
              sprintf(printBuffer, "  %d", i + 1); // Empty spaces to "blink" the label
          } else {
              sprintf(printBuffer, "%s%d", stateLabel, i + 1);
          }
          display.printValveStatus(i, printBuffer);
      }
    }
    if (now - lastDisplayUpdate >= 200) lastDisplayUpdate = now;
};
void Controler::check(){
  DateTime today = clock.now();
  // Permite ajustar la hora via Serial si es necesario
  clock.syncWithSerial();

  //Print hour
  display.printHour(clock.getHour());

  //Print dayOfTheWeek
  display.printDay(clock.getDayOfTheWeek());
  
  //Print estation
  display.printStation(clock.getStacion());
  
  if (useKeypad) {
      checkKeypad();
  } else {
      checkButtons();
      // Lógica para el botón de modo físico si se usa el método tradicional
      if (ModeButton != nullptr && ModeButton->read() == LOW) changeState();
  }
  this->checkBackLight();
  this->checkIrrigation(today);
};
