//Controller.cpp
//
#include <Arduino.h>
#include "Electrovalve.h"
#include "Controller.h"


Controller::Controller(Electrovalve** v_array, Button** b_array, uint8_t n_valves,  Pumb* PUmb, FlowSensor* flowSensor, Button* modeButton, SoilSensor* soilSensor, THSensor* thSensor):valves(v_array), valveButtons(b_array), numValves(n_valves), pumb(PUmb), flowSensor(flowSensor), ModeButton(modeButton), keypad(nullptr), useKeypad(false), soilSensor(soilSensor), thSensor(thSensor), state(1), backLightDuration(1), moistureInhibit(false){
};
Controller::Controller(Electrovalve** v_array, uint8_t n_valves, Pumb* PUmb, FlowSensor* flowSensor, CustomKeypad* keypad, SoilSensor* soilSensor, THSensor* thSensor) : valves(v_array), valveButtons(nullptr), numValves(n_valves), pumb(PUmb), flowSensor(flowSensor), ModeButton(nullptr), keypad(keypad), useKeypad(true), soilSensor(soilSensor), thSensor(thSensor), state(1), backLightDuration(1), moistureInhibit(false) {
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
    Serial.println(F("\nNumero de válvulas: ")); Serial.println(numValves);
    Serial.println(F("\n--- CONTROLADOR DE RIEGO REC ---"));
    Serial.println(F("Comandos serie disponibles:"));
    Serial.println(F("  S<valor> : Calibrar punto SECO (0%) del sensor de suelo"));
    Serial.println(F("  H<valor> : Calibrar punto HUMEDO (100%) del sensor de suelo"));
    Serial.println(F("  T<UnixTime,OffsetH> : Ajustar Fecha/Hora con Offset en horas (ej: T1781681651,2)"));
    Serial.println(F("  D    : Descargar historial de logs desde la EEPROM"));
    Serial.println(F("  R        : Borrar (Reset) todos los logs de la memoria"));
    Serial.println(F("  L        : Mostrar este menu de ayuda"));
    Serial.println(F("--------------------------------\n"));
}

void Controller::checkBackLight() {
if (display.getBackLight()){
    DateTime today = clock.now();
    if (today.unixtime() >= backLightTime.unixtime() + (uint32_t)backLightDuration * 60) {
        display.setOFF();
    }
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
      //Serial.print(F("VAL[")); Serial.print(i+1); Serial.print(F("] - State: ")); Serial.print(valves[i]->getState()); Serial.print(F(", wasActive: ")); Serial.println(wasActive ? "TRUE" : "FALSE");
      
      // Si el controlador está en STOP (0), forzamos el cierre de la válvula
      if (state == 0 || (state == 1 && correctionFactor <= 0)) {
          valves[i]->setOFF();
      } else {
          valves[i]->check(today, (state == 1) ? correctionFactor : 1.0f);
      }

      bool isNowActive = valves[i]->isActive();
      //Serial.print(F("VAL[")); Serial.print(i+1); Serial.print(F("] - isNowActive: ")); Serial.println(isNowActive ? "TRUE" : "FALSE");

      // Detectar Inicio de Riego
      if (!wasActive && isNowActive){
          //Serial.print(F("VAL[")); Serial.print(i+1); Serial.println(F("] - DETECTADO INICIO DE RIEGO!"));
          valves[i]->setStartTime(today.unixtime());
          valves[i]->setStartPulses(flowSensor->getPulses());
          valves[i]->setIsManualStart(valves[i]->getState() == 2); // Store if it started in manual mode
          //Serial.print(F("VAL[")); Serial.print(i+1); Serial.print(F("] - StartTime: ")); Serial.print(valves[i]->getStartTime()); Serial.print(F(", StartPulses: ")); Serial.print(valves[i]->getStartPulses()); Serial.print(F(", isManualStart: ")); Serial.println(valves[i]->getIsManualStart() ? "TRUE" : "FALSE");
      } 
      
      // Detectar Fin de Riego
      else if (wasActive && !isNowActive) {
          uint32_t startT = valves[i]->getStartTime();
          uint32_t endT = today.unixtime();
          
          // Calculamos la diferencia de pulsos y convertimos a litros
          uint32_t totalPulses = flowSensor->getPulses() - valves[i]->getStartPulses();
          // Convertimos la diferencia de pulsos a litros usando el factor K del sensor
          uint16_t totalLiters = (uint16_t)(totalPulses / (flowSensor->getKFactor() * 60.0)); 
          
          //Serial.print(F("VAL[")); Serial.print(i+1); Serial.println(F("] - DETECTADO FIN DE RIEGO!"));
          //Serial.print(F("VAL[")); Serial.print(i+1); Serial.print(F("] - Duracion: ")); Serial.print(endT - startT); Serial.print(F("s, Litros: ")); Serial.print(totalLiters); Serial.print(F(", Manual: ")); Serial.println(valves[i]->getIsManualStart() ? "TRUE" : "FALSE");
          // Si la válvula estaba en modo MANUAL (2), marcamos el ID para el log
          uint8_t valveIdForLog = i + 1;
          if (valves[i]->getIsManualStart()) valveIdForLog |= 0x80; // Marcamos el bit 7 si se inició manualmente
          
          clock.saveLog(valveIdForLog, totalLiters, startT, endT);
          clock.addLifetimeLiters(totalLiters); // Actualizar acumulado histórico
          
          // LIMPIEZA CRÍTICA: Resetear datos de seguimiento
          valves[i]->setStartTime(0);
          valves[i]->setStartPulses(0);
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

    // --- LOGS DE ACTIVIDAD DE LA BOMBA (Manual o Fugas) ---
    // Detectamos si hay flujo real en el sistema para identificar fugas
    bool isFlowing = flowSensor->getInstantFlow() > 0.1f;

    // Inicio de registro: 
    // 1. Hay flujo sin válvulas en AUTO (Posible FUGA)
    // 2. O estamos en modo MANUAL sin válvulas (Actividad MANUAL)
    if (pumb->getStartTime() == 0 && !anyActive) {
        if ((state == 1 && isFlowing) || (state == 2)) {
            pumb->setStartTime(today.unixtime());
            pumb->setStartPulses(flowSensor->getPulses());
            pumb->setIsManualStart(state == 2); // Identifica si es manual o fuga
        }
    }
    // Fin de registro: El flujo se detuvo, salimos de MANUAL o se abrió una válvula
    else if (pumb->getStartTime() != 0 && (anyActive || (state != 2 && !isFlowing))) {
        uint32_t startT = pumb->getStartTime();
        uint32_t endT = today.unixtime();
        uint32_t totalPulses = flowSensor->getPulses() - pumb->getStartPulses();
        uint16_t totalLiters = (uint16_t)(totalPulses / (flowSensor->getKFactor() * 60.0));
        
        uint8_t pumpIdForLog = pumb->getIsManualStart() ? 0x80 : ERR_LEAK; 
        
        if (totalLiters > 0 || (endT - startT) > 1) {
            clock.saveLog(pumpIdForLog, totalLiters, startT, endT);
            clock.addLifetimeLiters(totalLiters);
        }
        pumb->setStartTime(0);
    }

    if (now - lastDisplayUpdate >= 200) lastDisplayUpdate = now;
};

void Controller::checkCmds() {
    if (Serial.available() > 0) {
        char rawChar = Serial.read();
        if (isspace(rawChar)) return; // Ignorar saltos de línea o espacios iniciales

        char cmd = toupper(rawChar);
        switch (cmd) {
            case 'S':
            case 'H': {
                int val = Serial.parseInt();
                if (soilSensor != nullptr) {
                    if (cmd == 'S') {
                        soilSensor->calibrateDry(val);
                        Serial.print(F(">> Sensor Suelo: Calibracion SECO (0%) fijada en "));
                    } else {
                        soilSensor->calibrateWet(val);
                        Serial.print(F(">> Sensor Suelo: Calibracion HUMEDO (100%) fijada en "));
                    }
                    Serial.println(val);
                } else {
                    Serial.println(F(">> Error: Sensor de suelo no detectado o no configurado."));
                }
                break;
            }
            case 'T': {
            uint32_t unixTime = Serial.parseInt();
            // Leemos el siguiente entero como el offset (en horas).
            // Si no se envía (ej: T1781681651), parseInt devolverá 0 tras el timeout.
            int32_t offsetHours = Serial.parseInt(); 

            // Validamos que el timestamp sea razonable (posterior al 1 de enero de 2024)
            if (unixTime > 1704067200) { 
                int32_t totalOffset = offsetHours * 3600L;
                clock.adjustTime(DateTime(unixTime + totalOffset));

                Serial.print(F(">> RTC sincronizado. Offset aplicado: "));
                Serial.print(offsetHours); Serial.println(F("h."));
            } else {
                Serial.println(F(">> Error: Formato incorrecto. Use T<UnixTime>,<OffsetHoras>"));
            }
                break;
            }
            case 'D':
                clock.dumpLogsToSerial();
                break;
            case 'R':
                clock.clearMemory();
                Serial.println(F(">> Memoria de logs borrada con éxito."));
                break;
            case 'L':
                printHelp();
                break;
            default:
                Serial.print(F(">> Comando desconocido: ")); Serial.println(cmd);
                Serial.println(F(">> Pulse 'L' para ver el menú de ayuda."));
                break;
        }
        
        // Limpiar el buffer de entrada completamente tras procesar un comando.
        // Esto elimina comas sobrantes, saltos de línea o parámetros extra (como el 7º valor).
        delay(5); // Pequeña espera para asegurar que terminen de entrar caracteres por el serial
        while (Serial.available() > 0) {
            Serial.read();
        }
    }
}
float Controller::getCorrectionFactor() {
  int soilM = (soilSensor != nullptr) ? soilSensor->read() : 0;
  const int UPPER_THRESHOLD = 95; // Detener riego si sube de aquí
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
    float reduction = (float)(soilM - 50) / 45.0; // Ahora permite riego hasta el 95%
    factor *= (1.0f - reduction);
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
  
  // Debug de riego programado (cada 30 segundos)
  static uint32_t lastFactorLog = 0;
  if (now - lastFactorLog >= 30000) {
      Serial.print(F(">> Info Riego: Factor de Correccion Actual = ")); Serial.println(getCorrectionFactor());
      lastFactorLog = now;
  }

  if (useKeypad) {
      checkKeypad();
  } else {
     checkButtons();
      // Lógica para el botón de modo físico con detección de flanco e inversión
      if (ModeButton != nullptr) {
          static bool lastModeBtnState = HIGH;
          bool currentModeState = ModeButton->read();
          if (currentModeState == LOW && lastModeBtnState == HIGH) {
              if (display.getBackLight()) {
                  changeState();
              } else {
                  display.setON();
                  setBackLightTime(clock.now());
                  delay(200); 
              }
          }
          lastModeBtnState = currentModeState;
      }
  }
  this->checkBackLight();
  this->checkIrrigation(today, getCorrectionFactor());
};
