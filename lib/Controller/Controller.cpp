//Controller.cpp
//
#include <Arduino.h>
#include "Electrovalve.h"
#include "Controller.h"


Controller::Controller(Electrovalve** v_array, Button** b_array, uint8_t n_valves,  Pumb* PUmb, FlowSensor* flowSensor, Button* modeButton, SoilSensor* soilSensor, THSensor* thSensor):valves(v_array), valveButtons(b_array), numValves(n_valves), pumb(PUmb), flowSensor(flowSensor), ModeButton(modeButton), keypad(nullptr), useKeypad(false), soilSensor(soilSensor), thSensor(thSensor), state(2), backLightDuration(2), moistureInhibit(false){
};
Controller::Controller(Electrovalve** v_array, uint8_t n_valves, Pumb* PUmb, FlowSensor* flowSensor, CustomKeypad* keypad, SoilSensor* soilSensor, THSensor* thSensor) : valves(v_array), valveButtons(nullptr), numValves(n_valves), pumb(PUmb), flowSensor(flowSensor), ModeButton(nullptr), keypad(keypad), useKeypad(true), soilSensor(soilSensor), thSensor(thSensor), state(2), backLightDuration(2), moistureInhibit(false) {
};

void Controller::setAuto(){
  state = 1;
  display.printMode("AUTO");
};
void Controller::setManual(){
    state = 2;
    pumb->setON();
    display.printMode("MANU");
};
void Controller::setStop(){
    state = 0;
    pumb->setOFF();
    display.printMode("STOP");
};

void Controller::init(){
    if (useKeypad && keypad != nullptr) {
        keypad->init();
    }
    if (thSensor != nullptr) {
        thSensor->init();
    }
    printHelp();
};

void Controller::printHelp() {
    Serial.println(F("\n--- CONTROLADOR DE RIEGO REC ---"));
    Serial.println(F("Comandos serie disponibles:"));
    Serial.println(F("  S<valor> : Calibrar punto SECO (0%) del sensor de suelo"));
    Serial.println(F("  H<valor> : Calibrar punto HUMEDO (100%) del sensor de suelo"));
    Serial.println(F("  T<Y,M,D,h,m,s> : Ajustar Fecha/Hora (ej: T2024,05,22,10,30,00)"));
    Serial.println(F("  L o D    : Descargar historial de logs desde la EEPROM"));
    Serial.println(F("  R        : Borrar (Reset) todos los logs de la memoria"));
    Serial.println(F("  ?        : Mostrar este menu de ayuda"));
    Serial.println(F("--------------------------------\n"));
}

void Controller::checkBackLight() {
if (display.getBackLight()){
    DateTime today = clock.now();
    int nowMinutes = today.hour() * 60 + today.minute();
    int backLightMinuts = backLightTime.hour() * 60 + backLightTime.minute();
    if (!((nowMinutes >= backLightMinuts) && (nowMinutes < (backLightMinuts + backLightDuration)))){
     display.setOFF();
    };
  };
};

void Controller::changeState() {
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
void Controller::setBackLightTime(DateTime time) {
  this->backLightTime = time;
};

void Controller::checkButtons() {
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

void Controller::checkKeypad() {
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

void Controller::checkIrrigation(DateTime today, float correctionFactor) {
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
      if (state == 0 || (state == 1 && correctionFactor <= 0)) {
          valves[i]->setOFF();
      } else {
          valves[i]->check(today, (state == 1) ? correctionFactor : 1.0f);
      }

      bool isNowActive = valves[i]->isActive();

      // Detectar Inicio de Riego
      if (!wasActive && isNowActive) {
          valves[i]->setStartTime(today.unixtime());
          valves[i]->setStartPulses(flowSensor->getPulses());
      } 
      // Detectar Fin de Riego
      else if (wasActive && !isNowActive) {
          uint32_t startT = valves[i]->getStartTime();
          uint32_t endT = today.unixtime();
          
          // Calculamos la diferencia de pulsos y convertimos a litros
          uint32_t totalPulses = flowSensor->getPulses() - valves[i]->getStartPulses();
          // Convertimos la diferencia de pulsos a litros usando el factor K del sensor
          uint16_t totalLiters = (uint16_t)(totalPulses / (flowSensor->getKFactor() * 60.0)); 
          clock.saveLog(i + 1, totalLiters, startT, endT);
          clock.addLifetimeLiters(totalLiters); // Actualizar acumulado histórico
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

void Controller::checkCmds() {
    if (Serial.available() > 0) {
        char cmd = Serial.read(); 

        // 1. Comandos del Sensor de Suelo (S: Seco, H: Humedo)
        if (soilSensor != nullptr && (cmd == 'S' || cmd == 's' || cmd == 'H' || cmd == 'h')) {
            int val = Serial.parseInt();
            if (cmd == 'S' || cmd == 's') {
                soilSensor->calibrateDry(val);
                Serial.print(F(">> Sensor Suelo: Calibracion SECO (0%) fijada en "));
            } else {
                soilSensor->calibrateWet(val);
                Serial.print(F(">> Sensor Suelo: Calibracion HUMEDO (100%) fijada en "));
            }
            Serial.println(val);
        } 
        // 2. Comandos del Reloj (T: Tiempo)
        else if (cmd == 'T' || cmd == 't') {
            int year = Serial.parseInt();
            if (year >= 2024) {
                int month = Serial.parseInt();
                int day = Serial.parseInt();
                int hour = Serial.parseInt();
                int minute = Serial.parseInt();
                int second = Serial.parseInt();
                clock.adjustTime(DateTime(year, month, day, hour, minute, second));
                Serial.println(F(">> RTC sincronizado con éxito."));
            }
        }
        // 3. Comandos de Logs (L: Logs, D: Dump)
        else if (cmd == 'L' || cmd == 'l' || cmd == 'D' || cmd == 'd') {
            clock.dumpLogsToSerial();
        }
        // 4. Reset logs
        else if (cmd == 'R' || cmd == 'r') {
            clock.clearMemory();
            Serial.println(F(">> Memoria de logs borrada con exito."));
        }
        // 4. Ayuda
        else if (cmd == '?') {
            printHelp();
        }
    }
}
float Controller::getCorrectionFactor() {
  int soilM = (soilSensor != nullptr) ? soilSensor->read() : 0;
  const int UPPER_THRESHOLD = 85; // Detener riego si sube de aquí
  const int LOWER_THRESHOLD = 80; // Permitir riego solo si baja de aquí

  // 1. Lógica de Histéresis
  if (soilM >= UPPER_THRESHOLD) {
    moistureInhibit = true;
  } else if (soilM < LOWER_THRESHOLD) {
    moistureInhibit = false;
  }

  // Si estamos inhibidos por exceso de humedad (lluvia), el factor es 0
  if (moistureInhibit) return 0.0;

  float factor = 1.0;

  // 2. Ajustes basados en condiciones ambientales (si el sensor está disponible)
  if (thSensor != nullptr) {
    float temp = thSensor->readTemperature();
    float hum = thSensor->readHumidity();

    // Aumentar un 3% por cada grado sobre 25°C, reducir si baja de 15°C
    if (temp > 25.0) factor += (temp - 25.0) * 0.03;
    else if (temp < 15.0) factor -= (15.0 - temp) * 0.02;

    // Ajustar por humedad relativa del aire
    if (hum > 75.0) factor -= 0.2;      // Ambiente muy húmedo -> menos riego
    else if (hum < 30.0) factor += 0.2; // Ambiente muy seco -> más riego
  }

  // 3. Reducción progresiva según humedad del suelo (rango de 50% a 85%)
  if (soilM > 50) {
    float reduction = (float)(soilM - 50) / 35.0; // Escala de 0.0 a 1.0
    factor *= (1.0 - reduction);
  }

  // Limitar el factor entre 0.0 y un máximo de 2.0 (doble de tiempo) para seguridad
  if (factor < 0.0) factor = 0.0;
  if (factor > 2.0) factor = 2.0;
  return factor;
};

void Controller::check() {
  DateTime today = clock.now();
  uint32_t now = millis();
  static uint32_t lastSlowUpdate = 0;

  // Procesar comandos recibidos por puerto serie
  checkCmds();

  // Actualizar información estática/lenta solo cada 1 segundo para evitar saturar el I2C
  if (now - lastSlowUpdate >= 1000) {
      display.printHour(clock.getHour());
      display.printDay(clock.getDayOfTheWeek());
      display.printStation(clock.getStacion());
      if (soilSensor != nullptr) {
          display.printSoilMoisture(soilSensor->read());
      }
      if (thSensor != nullptr) {
          // Se asume que THSensor implementa los métodos estándar de la librería DHT
          display.printTemperature((int)thSensor->readTemperature());
          display.printHumidity((int)thSensor->readHumidity());
      }
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
  this->checkIrrigation(today, getCorrectionFactor());
};
