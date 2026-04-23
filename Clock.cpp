//Clock.cpp
#include "Clock.h"

Clock::Clock(): _nextIndex(0), _isFull(false){
    init();
};

bool Clock::init(){
  Serial.println(__DATE__);
  Serial.println(__TIME__);
  if (!RTC.begin()){
    Serial.println("Couldn't find RTC");
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
  // Leer estado actual
  readEEPROM(_ptrAddr, (byte*)&_nextIndex, sizeof(_nextIndex));
  readEEPROM(_fullFlagAddr, (byte*)&_isFull, sizeof(_isFull));
  
  if (_nextIndex >= _maxEntries) { // Fail-safe por si la EEPROM es nueva
    clearMemory();
  }
  return true;
};

void Clock::saveLog(uint8_t valve, uint16_t liters) {
  LogEntry newLog = {RTC.now().unixtime(), liters, valve};

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
  errorLog.timestamp = RTC.now().unixtime();
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
  DateTime today = now();
  sprintf(hourBuffer, "%02d:%02d", today.hour(), today.minute());
  return hourBuffer;
};

const char* Clock::getDayOfTheWeek(){
  DateTime today = now();
  return D[today.dayOfTheWeek()];
};
const char* Clock::getStacion(){
  DateTime today = now();
  return E[today.month()-1];
};
void Clock::dumpLogsToSerial() {
    uint16_t total = getCount();
    Serial.println(F("\n--- INICIO VOLCADO EEPROM ---"));
    Serial.print(F("Entradas encontradas: ")); Serial.println(total);
    Serial.println(F("Fecha(Unix) | Valv/Err | Dato"));
    Serial.println(F("--------------------------------"));

    for (uint16_t i = 0; i < total; i++) {
        LogEntry entry = getLog(i);
        
        DateTime d(entry.timestamp); // Convertimos el Unix time a objeto DateTime

        // Imprimir Fecha: DD/MM/AAAA
        Serial.print(d.day()); Serial.print('/');
        Serial.print(d.month()); Serial.print('/');
        Serial.print(d.year()); Serial.print(F(" "));

        // Imprimir Hora: HH:MM
        if(d.hour() < 10) Serial.print('0');
        Serial.print(d.hour()); Serial.print(':');
        if(d.minute() < 10) Serial.print('0');
        Serial.print(d.minute());

        Serial.print(F(" | "));
        
        // Si el valor es muy alto (ej: > 200), asumimos que es un código de error
        if (entry.valve >= 200) {
            Serial.print(F("ERROR: "));
        } else {
            Serial.print(F("Válvula: "));
        }
        
        Serial.print(entry.valve);
        Serial.print(F(" | "));
        Serial.print(entry.liters);
        Serial.println(F(" units"));
    }
    Serial.println(F("--- FIN DEL VOLCADO ---\n"));
};
