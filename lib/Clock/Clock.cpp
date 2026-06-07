//Clock.cpp
#include <Arduino.h>
#include "Clock.h"

Clock::Clock(): _nextIndex(0), _isFull(false){
    // No llamar a init() aquí. Dejar que setup() lo haga.
};

bool Clock::init(){
  Serial.println(__DATE__);
  Serial.println(__TIME__);
  if (!RTC.begin()){
    Serial.println(F("Couldn't find RTC"));
    Serial.flush();
    while (1) delay(10);
    return false;
  }else{  
    Serial.println("Find RTC");
     // RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  if (RTC.lostPower()) {
    // Solo ajustar si se perdió la hora
    RTC.adjust(DateTime(__DATE__, __TIME__));
  };

  // Leer estado actual de la EEPROM
  readEEPROM(_ptrAddr, (byte*)&_nextIndex, sizeof(_nextIndex));
  
  uint8_t tempFull;
  readEEPROM(_fullFlagAddr, &tempFull, 1);

  // Fail-safe: Si el índice es mayor al máximo o el flag no es 0 ni 1 (EEPROM nueva/sucia)
  if (_nextIndex >= _maxEntries || tempFull > 1) {
    clearMemory();
  } else {
    _isFull = (tempFull == 1);
  }

  return true;
};

void Clock::saveLog(uint8_t valve, uint16_t liters, uint32_t startT, uint32_t endT) {
  LogEntry newLog = {startT, endT, liters, valve};

  // 1. Escribir en la posición actual
  uint16_t addr = _startDataAddr + (_nextIndex * sizeof(LogEntry));
  writeEEPROM(addr, (byte*)&newLog, sizeof(LogEntry));

  // 2. Avanzar el puntero
  _nextIndex++;
  if (_nextIndex >= _maxEntries) {
    _nextIndex = 0;   // Volvemos al principio
    _isFull = true;   // Marcamos que ya empezamos a sobrescribir
    writeEEPROM(_fullFlagAddr, (byte*)&_isFull, sizeof(_isFull));
  }

  // 3. Guardar el nuevo puntero
  writeEEPROM(_ptrAddr, (byte*)&_nextIndex, sizeof(_nextIndex));
}

void Clock::saveError(uint8_t errorCode, uint16_t detail) {
  LogEntry errorLog;
  errorLog.startTime = RTC.now().unixtime();
  errorLog.endTime = errorLog.startTime;
  errorLog.valve = errorCode; // Aquí guardamos ERR_LEAK o ERR_DRY_RUN
  errorLog.liters = detail;    // Podemos guardar el caudal que detectamos

  uint16_t addr = _startDataAddr + (_nextIndex * sizeof(LogEntry));
  writeEEPROM(addr, (byte*)&errorLog, sizeof(LogEntry));

  _nextIndex++;
  if (_nextIndex >= _maxEntries) {
    _nextIndex = 0;
    _isFull = true;
    writeEEPROM(_fullFlagAddr, (byte*)&_isFull, sizeof(_isFull));
  }
  writeEEPROM(_ptrAddr, (byte*)&_nextIndex, sizeof(_nextIndex));
}

uint16_t Clock::getCount() {
  return _isFull ? _maxEntries : _nextIndex;
}

LogEntry Clock::getLog(uint16_t logicalIndex) {
  uint16_t physicalIndex;
  
  if (!_isFull) {
    physicalIndex = logicalIndex;
  } else {
    // Si está lleno, el más antiguo es el que está en _nextIndex
    physicalIndex = (_nextIndex + logicalIndex) % _maxEntries;
  }

  LogEntry data;
  uint16_t addr = _startDataAddr + (physicalIndex * sizeof(LogEntry));
  readEEPROM(addr, (byte*)&data, sizeof(LogEntry));
  return data;
}

void Clock::clearMemory() {
  _nextIndex = 0;
  _isFull = false;
  writeEEPROM(_ptrAddr, (byte*)&_nextIndex, sizeof(_nextIndex));
  writeEEPROM(_fullFlagAddr, (byte*)&_isFull, sizeof(_isFull));
}

// --- Funciones de bajo nivel para la EEPROM AT24C32 ---

void Clock::writeEEPROM(uint16_t address, byte* data, uint16_t len) {
  Wire.beginTransmission(_eepromAddr);
  Wire.write((int)(address >> 8));   // MSB
  Wire.write((int)(address & 0xFF)); // LSB
  for (uint16_t i = 0; i < len; i++) {
    Wire.write(data[i]);
  }
  Wire.endTransmission();
  delay(5); // Tiempo de escritura de la EEPROM
}

void Clock::readEEPROM(uint16_t address, byte* data, uint16_t len) {
  Wire.beginTransmission(_eepromAddr);
  Wire.write((int)(address >> 8));
  Wire.write((int)(address & 0xFF));
  Wire.endTransmission();
  
  Wire.requestFrom(_eepromAddr, (uint8_t)len);
  for (uint16_t i = 0; i < len; i++) {
    if (Wire.available()) data[i] = Wire.read();
  }
}

char* Clock::getHour(){
  
  static char hourBuffer[6]; // 'static' ensures the memory persists after function ends
  DateTime today = RTC.now();
  sprintf(hourBuffer, "%02d:%02d", today.hour(), today.minute());
  return hourBuffer;
};

const char* Clock::getDayOfTheWeek(){
  DateTime today = RTC.now();
  return D[today.dayOfTheWeek()];
};
const char* Clock::getStacion(){
  DateTime today = RTC.now();
  return E[today.month()-1];
};
void Clock::dumpLogsToSerial() {
    uint16_t total = getCount();
    Serial.println(F("\n--- HISTORIAL DE RIEGOS Y ERRORES ---"));
    Serial.print(F("Registros encontrados: ")); Serial.println(total);
    Serial.println(F("Fecha      | Inicio | Fin   | Durac. | ID   | Consumo"));
    Serial.println(F("-----------|--------|-------|--------|------|---------"));

    for (uint16_t i = 0; i < total; i++) {
        LogEntry entry = getLog(i);
        
        DateTime start(entry.startTime);
        DateTime end(entry.endTime);
        uint32_t durationSeconds = entry.endTime - entry.startTime;

        // Imprimir Fecha: DD/MM/AAAA
        if(start.day() < 10) Serial.print('0');
        Serial.print(start.day()); Serial.print('/');
        if(start.month() < 10) Serial.print('0');
        Serial.print(start.month()); Serial.print('/');
        Serial.print(start.year());
        Serial.print(F(" | "));

        // Hora Inicio
        if(start.hour() < 10) Serial.print('0');
        Serial.print(start.hour()); Serial.print(':');
        if(start.minute() < 10) Serial.print('0');
        Serial.print(start.minute());
        Serial.print(F("  | "));

        // Hora Fin
        if(end.hour() < 10) Serial.print('0');
        Serial.print(end.hour()); Serial.print(':');
        if(end.minute() < 10) Serial.print('0');
        Serial.print(end.minute());
        Serial.print(F(" | "));

        // Duración (ej: 120s)
        Serial.print(durationSeconds/60); Serial.print(F("m"));
        Serial.print(F("\t | "));
        
        // Identificador (Válvula o Error)
        if (entry.valve == ERR_LEAK) Serial.print(F("FUGA "));
        else if (entry.valve == ERR_DRY_RUN) Serial.print(F("SECO "));
        else { Serial.print(F("VAL")); Serial.print(entry.valve); }
        
        Serial.print(F(" | "));
        Serial.print(entry.liters);
        Serial.println(F(" L"));
    }
    Serial.println(F("----------------------------------------------------\n"));
};
