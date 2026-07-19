//Controller.cpp
//
#include <Arduino.h>
#include "Electrovalve.h"
#include "Controller.h"


Controller::Controller(Electrovalve** v_array, Button** b_array, uint8_t n_valves,  Pumb* PUmb, Button* modeButton, SoilSensor* soilSensor, THSensor* thSensor)
    : valves(v_array), valveButtons(b_array), numValves(n_valves), pumb(PUmb), ModeButton(modeButton), keypad(nullptr), useKeypad(false), soilSensor(soilSensor), thSensor(thSensor), state(0), backLightDuration(1), moistureInhibit(false),
      leakStartTime(0), leakStartPulses(0), manualStartTime(0), manualStartPulses(0), noFlowStartTime(0), lastActiveTime(0) {
};

Controller::Controller(Electrovalve** v_array, uint8_t n_valves, Pumb* PUmb, CustomKeypad* keypad, SoilSensor* soilSensor, THSensor* thSensor)
    : valves(v_array), valveButtons(nullptr), numValves(n_valves), pumb(PUmb), ModeButton(nullptr), keypad(keypad), useKeypad(true), soilSensor(soilSensor), thSensor(thSensor), state(0), backLightDuration(1), moistureInhibit(false),
      leakStartTime(0), leakStartPulses(0), manualStartTime(0), manualStartPulses(0), noFlowStartTime(0), lastActiveTime(0) {
};

// Funcion para cambiar el estado del controlador a AUTO.
// Viene de STOP. Si había flujo mientras estaba en STOP, se registra un error de fuga.
void Controller::setAuto(){ 
  state = 1;
  display.printMode("AUTO");
  pumb->setOFF();
  pumb->setState(1);
};

// Funcion para cambiar el estado del controlador a MANUAL.
// Viene de AUTO. Si comprueba si en auto había flujo sin electrovalvulas encendidas y da el log de error. 
//registra el tiempo y pulsos de inicio para el log de riego manual.
void Controller::setManual(){
    state = 2;
    display.printMode("MANU");
    for (uint8_t i = 0; i < numValves; i++) {
        valves[i]->setOFF();        
    }
    pumb->setStartTime(clock.now().unixtime());
    pumb->setStartPulses(flowSensor.getPulses());
    pumb->setIsManualStart(true);
    pumb->setON();
    pumb->setState(2);
};

// Funcion para cambiar el estado del controlador a STOP y apaga todas las electrovalvulas.

void Controller::setStop(){
    state = 0;
    display.printMode("STOP");
    
    for (uint8_t i = 0; i < numValves; i++) { 
         valves[i]->setOFF();      
    }
    pumb->setOFF();
    pumb->setState(0);
    lastActiveTime = clock.now().unixtime();
};

void Controller::init(){
    for (uint8_t i = 0; i < numValves; i++) {
        valves[i]->setLastValveStates(1); // Default to AUTO mode for electrovalves
        valves[i]->setValveNoFlowStartTime(0);
    }

    if (useKeypad && keypad != nullptr) {
        keypad->init();
    }
    if (thSensor != nullptr) {
        thSensor->init();
    }
    printHelp();
    setStop();
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
    Serial.println(F("  C        : Resetear contador de litros totales de vida"));
    Serial.println(F("  L        : Mostrar este menu de ayuda"));
    Serial.println(F("--------------------------------\n"));
};

void Controller::checkBackLight() {
if (display.getBackLight()){
    DateTime today = clock.now();
    if (today.unixtime() >= backLightTime.unixtime() + (uint32_t)backLightDuration * 60) {
        display.setOFF();
    }
  };
};

void Controller::changeState() {
  DateTime today = clock.now();
  //Serial.print(F(">> Estado actual del controlador: ")); Serial.println(state);
  switch (state) {
    case 0: { // De STOP a AUTO
        checkAndSaveActiveLeak(today);
        setAuto();
        return;
      }
      case 1: { // De AUTO a MANUAL
        checkAndSaveActiveLeak(today);
        setManual();
        return;
      }
      case 2: { // De MANUAL a STOP
        checkAndSaveActiveLeak(today);
        uint32_t endT = today.unixtime();
        uint32_t totalPulses = flowSensor.getPulses() - pumb->getStartPulses();
        uint16_t totalLiters = (uint16_t)(totalPulses / (flowSensor.getKFactor() * 60.0));
        clock.saveLog(0x80, totalLiters, pumb->getStartTime(), endT);
        clock.addLifetimeLiters(totalLiters);
        setStop();
        return;
      }
  }
};
void Controller::setBackLightTime(DateTime time) {
  this->backLightTime = time;
};

void Controller::checkButtons() {
  bool anyButtonPressed = false;
    char printBuffer[20]; // Buffer para sprintf
    //static bool lastValveBtnStates[10] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};

   // if (state == 0) return; // Si estamos en STOP, no procesamos botones de válvula
    
    // 1. Recorremos todas las válvulas con un bucle
    for (uint8_t i = 0; i < numValves; i++) {
        bool currentBtnState = valveButtons[i]->read();
        
        // Detectamos el flanco de bajada (de HIGH a LOW) con resistencia Pull-up
        if (currentBtnState == LOW && valveButtons[i]->getLastState() == HIGH) {
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
        valveButtons[i]->setLastState(currentBtnState);
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
        if (key >= '0' && key <= '9') {
            uint8_t i = key - '0';
            if (i < numValves) {
                valves[i]->changeState();
                char printBuffer[20];
                sprintf(printBuffer, "%s%d", valves[i]->getLabelState(), valves[i]->getId());
                display.printValveStatus(i, printBuffer);
            }
        }

        
    }
};

void Controller::checkAndSaveActiveLeak(DateTime today) { // Función para verificar si hubo flujo mientras el sistema estaba en STOP y registrar un posible error de fuga
    if (leakStartTime != 0) {
        uint32_t endT = today.unixtime();
        uint32_t totalPulses = flowSensor.getPulses() - leakStartPulses;
        uint16_t totalLiters = (uint16_t)(totalPulses / (flowSensor.getKFactor() * 60.0));
        if (totalLiters >= 2) {
            clock.saveLog(ERR_LEAK, totalLiters, leakStartTime, endT);
            clock.addLifetimeLiters(totalLiters);
        }
        Serial.print(F(">> Instant flow: "));Serial.print(flowSensor.getInstantFlow()); Serial.print(F(" L/min, Total Pulses: ")); Serial.print(totalPulses); Serial.print(F(", Total Liters: ")); Serial.println(totalLiters);
        leakStartTime = 0;
        leakStartPulses = 0;
    }
};

void Controller::stopController() {
     for (uint8_t i = 0; i < numValves; i++) { 
          valves[i]->setOFF();
     }
    
    DateTime today = clock.now();
    bool isFlowing = flowSensor.getInstantFlow() > 0.1f;
    if (today.unixtime() - lastActiveTime < 5) {
        return; // Ignore flow sensor deceleration time
    }
    
    if (isFlowing) {
        if (leakStartTime == 0) {
            leakStartTime = today.unixtime();
            leakStartPulses = flowSensor.getPulses();
        }
    } else {
        checkAndSaveActiveLeak(today);
    }
};

void Controller::autoController(DateTime today, float correctionFactor) {
    bool anyActive = false;
    
    // FALTA la lectura de los botones de las válvulas, si es que se usan botones físicos para cambiar el estado de las válvulas y cambiar el estado de las electorválvulas. 
    // En el controlador en estado Auto las valvulas puedes cambiar de estado (stop -> auto -> manual)
    checkButtons();
    // Process each valve
    for (uint8_t i = 0; i < numValves; i++) {
        bool wasActive = valves[i]->isActive();
        uint8_t prevMode = valves[i]->getLastValveStates();
        uint8_t currMode = valves[i]->getState();
        
        // Handle valve mode transition
        if (currMode != prevMode) {
            if (prevMode == 2) { // Left MANUAL mode
                uint32_t endT = today.unixtime();
                uint32_t totalPulses = flowSensor.getPulses() - valves[i]->getStartPulses();
                uint16_t totalLiters = (uint16_t)(totalPulses / (flowSensor.getKFactor() * 60.0));
                
                clock.saveLog((i + 1) | 0x80, totalLiters, valves[i]->getStartTime(), endT);
                clock.addLifetimeLiters(totalLiters);
                
                valves[i]->setStartTime(0);
                valves[i]->setStartPulses(0);
                valves[i]->setValveNoFlowStartTime(0);
                
                valves[i]->setLastValveStates(currMode);
                return; // Exit completely to transition safely
            }
            valves[i]->setLastValveStates(currMode);
        }
        
        if (currMode == 2 && prevMode != 2) { // Entering MANUAL mode
            valves[i]->setIsManualStart(true);
            valves[i]->setStartTime(today.unixtime());
            valves[i]->setStartPulses(flowSensor.getPulses());
            valves[i]->setValveNoFlowStartTime(0);
        }
        
        // Execute state logic
        if (currMode == 0) { // STOP
            valves[i]->setOFF();
            if (wasActive) { //
                valves[i]->setStartTime(0);
                valves[i]->setStartPulses(0);
                valves[i]->setValveNoFlowStartTime(0);
            }
        } else if (currMode == 1) { // AUTO
            if (correctionFactor <= 0) {
                valves[i]->setOFF();
            } else {
                valves[i]->check(today, correctionFactor);
            }
            
            bool isNowActive = valves[i]->isActive();
            if (!wasActive && isNowActive) {
                valves[i]->setStartTime(today.unixtime());
                valves[i]->setStartPulses(flowSensor.getPulses());
                valves[i]->setValveNoFlowStartTime(0);
            } else if (wasActive && isNowActive) {
                bool isFlowing = flowSensor.getInstantFlow() > 0.1f;
                if (!isFlowing) {
                    if (valves[i]->getValveNoFlowStartTime() == 0) {
                    valves[i]->setValveNoFlowStartTime(today.unixtime());
                    } else if (today.unixtime() - valves[i]->getValveNoFlowStartTime() >= 5) {
                        // Dry running!
                        uint32_t endT = today.unixtime();
                        clock.saveLog(ERR_DRY_RUN, 0, endT, endT);
                        
                        valves[i]->setOFF();
                        valves[i]->setStartTime(0);
                        valves[i]->setStartPulses(0);
                        valves[i]->setValveNoFlowStartTime(0);
                    }
                } else {
                    valves[i]->setValveNoFlowStartTime(0);
                }
            } else if (wasActive && !isNowActive) {
                uint32_t endT = today.unixtime();
                uint32_t totalPulses = flowSensor.getPulses() - valves[i]->getStartPulses();
                uint16_t totalLiters = (uint16_t)(totalPulses / (flowSensor.getKFactor() * 60.0));
                
                clock.saveLog(valves[i]->getId(), totalLiters, valves[i]->getStartTime(), endT);
                clock.addLifetimeLiters(totalLiters);
                
                valves[i]->setStartTime(0);
                valves[i]->setStartPulses(0);
                valves[i]->setValveNoFlowStartTime(0);
            }
        } else if (currMode == 2) { // MANUAL (valve)
            valves[i]->check(today, 1.0f); // Ensures it stays ON
            
            bool isNowActive = valves[i]->isActive();
            if (!wasActive && isNowActive) {
                valves[i]->setStartTime(today.unixtime());
                valves[i]->setStartPulses(flowSensor.getPulses());
                valves[i]->setValveNoFlowStartTime(0);
            } else if (wasActive && isNowActive) {
                bool isFlowing = flowSensor.getInstantFlow() > 0.1f;
                if (!isFlowing) {
                    if (valves[i]->getValveNoFlowStartTime() == 0) {
                        valves[i]->setValveNoFlowStartTime(today.unixtime());
                    } else if (today.unixtime() - valves[i]->getValveNoFlowStartTime() >= 5) {
                        // Dry run in valve manual mode!
                        uint32_t endT = today.unixtime();
                        uint32_t totalPulses = flowSensor.getPulses() - valves[i]->getStartPulses();
                        uint16_t totalLiters = (uint16_t)(totalPulses / (flowSensor.getKFactor() * 60.0));
                        
                        clock.saveLog(valves[i]->getId() | 0x80, totalLiters, valves[i]->getStartTime(), valves[i]->getValveNoFlowStartTime());
                        clock.addLifetimeLiters(totalLiters);
                        
                        clock.saveLog(ERR_DRY_RUN, 0, endT, endT);
                        
                        valves[i]->setOFF();
                        valves[i]->setStartTime(0);
                        valves[i]->setStartPulses(0);
                        valves[i]->setValveNoFlowStartTime(0);
                        
                        setStop();
                        return;
                    }
                } else {
                    valves[i]->setValveNoFlowStartTime(0);
                }
            }
        }
        
        if (valves[i]->isActive()) {
            anyActive = true;
        }
    }
    
    // Master pump control
    if (anyActive) {
        pumb->setON();
    } else {
        pumb->setOFF();
    }
    
    // Leak logging
    bool isFlowing = flowSensor.getInstantFlow() > 0.1f;
    if (anyActive) {
        lastActiveTime = today.unixtime();
        checkAndSaveActiveLeak(today);
    } else {
        if (today.unixtime() - lastActiveTime >= 5) {
            if (isFlowing) {
                if (leakStartTime == 0) {
                    leakStartTime = today.unixtime();
                    leakStartPulses = flowSensor.getPulses();
                }
            } else {
                checkAndSaveActiveLeak(today);
            }
        }
    }
};

void Controller::manualController() {
    pumb->setON();
    for (uint8_t i = 0; i < numValves; i++) {
        valves[i]->setOFF();
    }
    
    bool isFlowing = flowSensor.getInstantFlow() > 0.1f;
    DateTime today = clock.now();
    uint32_t nowT = today.unixtime();
    
    lastActiveTime = nowT; // Record the last time manual pump was active
    
    if (!isFlowing) {
        if (pumb->getValveNoFlowStartTime() == 0) {
            pumb->setValveNoFlowStartTime(nowT);
        } else if (nowT - pumb->getValveNoFlowStartTime() >= 5) {
            // Log manual run up to stop of flow
            uint32_t totalPulses = flowSensor.getPulses() - pumb->getStartPulses();
            uint16_t totalLiters = (uint16_t)(totalPulses / (flowSensor.getKFactor() * 60.0));
            clock.saveLog(0x80, totalLiters, pumb->getStartTime(), pumb->getValveNoFlowStartTime());
            clock.addLifetimeLiters(totalLiters);
            
            // Log dry run error
            clock.saveLog(ERR_DRY_RUN, 0, nowT, nowT);
            
            pumb->setStartTime(0);
            pumb->setStartPulses(0);
            setStop();
        }
    } else {
        pumb->setValveNoFlowStartTime(0);
    }
};

void Controller::check() {
  DateTime today = clock.now();
  uint32_t now = millis();
  static uint32_t lastSlowUpdate = 0; // Timestamp for the last slow update (1 second interval)

  // Procesar comandos recibidos por puerto serie
  checkCmds();

  // Actualizar información estática/lenta solo cada 1 segundo para evitar saturar el I2C
  if (now - lastSlowUpdate >= 1000) {
      display.printHour(clock.getHour());
      display.printDay(clock.getDayOfTheWeek());
      display.printStation(clock.getStacion());
      if (soilSensor != nullptr) {
          display.printSoilMoisture(soilSensor->getMoisturePercentage());
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
      //Serial.print(F(">> Info Riego: Factor de Correccion Actual = ")); Serial.println(getCorrectionFactor());
      lastFactorLog = now;
  }

  if (useKeypad) {
      checkKeypad();
  } else {
      //checkButtons();
      // Lógica para el botón de modo físico con detección de flanco e inversión
      if (ModeButton != nullptr) { // Asegurarse de que el botón de modo esté definido
          // static bool lastModeBtnState = HIGH;
          bool currentModeState = ModeButton->read();
          //Serial.print(F(">> Estado Boton Modo: ")); Serial.println(currentModeState);
          if (currentModeState == LOW && ModeButton->getLastState() == HIGH) {
              if (display.getBackLight()) {
                  changeState();
              } else {
                  display.setON();
                  setBackLightTime(clock.now());
                  delay(200); 
              }
          }
          ModeButton->setLastState(currentModeState);
      }
  }
  this->checkBackLight();
  
  if (state == 0) {
      stopController();
  } else if (state == 1) {
      autoController(today, getCorrectionFactor());
  } else if (state == 2) {
      manualController();
  }

  // Update display (flow and valve statuses)
  static uint32_t lastDisplayUpdate = 0;
  if (now - lastDisplayUpdate >= 200) {
      display.printFlow(flowSensor.getInstantFlow());
      
      bool blinkVisible = (now / 500) % 2;
      char printBuffer[20];
      for (uint8_t i = 0; i < numValves; i++) {
          if (valves[i]->isActive() && !blinkVisible) {
              sprintf(printBuffer, "  ");
          } else {
              sprintf(printBuffer, "%s%d", valves[i]->getLabelState(), i + 1);
          }
          display.printValveStatus(i, printBuffer);
      }
      lastDisplayUpdate = now;
  }
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
            case 'C':
                clock.resetLifetimeLiters();
                Serial.println(F(">> Contador de litros de vida total reseteado a 0 L."));
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
};

float Controller::getCorrectionFactor() {
  int soilM = (soilSensor != nullptr) ? soilSensor->getMoisturePercentage() : 0;
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


