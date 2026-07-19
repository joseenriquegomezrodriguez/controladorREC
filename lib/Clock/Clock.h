//Clock.h
#ifndef Clock_h
#define Clock_h
#include <Arduino.h>
#include <RTClib.h>

// Estructura (11 bytes totales)
struct __attribute__((packed)) LogEntry {
  uint32_t startTime; // 4 bytes
  uint32_t endTime;   // 4 bytes
  uint16_t liters;    // 2 bytes
  uint8_t valve;      // 1 byte (ID de válvula o código de error)
};
// Códigos de error especiales para el campo 'valve'
#define ERR_LEAK    255  // Fuga (flujo sin válvula abierta)
#define ERR_DRY_RUN 254  // Bomba en seco (válvula abierta sin flujo)

class Clock {
    private:
        RTC_DS3231 RTC;
        const char D[7][3] = {"DG","DL","DT","DC","DJ","DV","DS"};
        const char E[12][4] = {"HIV","HIV","HIV","PRI","PRI","PRI","EST","EST","EST","TAR","TAR","TAR"};
        const uint8_t _eepromAddr = 0x57;
        // Direcciones de control en EEPROM
        const uint16_t _ptrAddr = 0;       // Donde toca escribir el siguiente (2 bytes)
        const uint16_t _fullFlagAddr = 2;  // Indica si ya se llenó una vez (1 byte)
        const uint16_t _startDataAddr = 4; // Inicio de los registros
        const uint16_t _lifetimeAddr = 4080; // Dirección para litros totales en RTC EEPROM
    
        const uint16_t _maxEntries = 370;  // Ajustado para 11 bytes por entrada (4070 bytes)
    
        uint16_t _nextIndex;
        bool _isFull;
        
        void writeEEPROM(uint16_t address, byte* data, uint16_t len);
        void readEEPROM(uint16_t address, byte* data, uint16_t len);
    public:
        Clock();
        bool init();
        
        char* getHour();
        const char* getDayOfTheWeek();
        const char* getStacion();
        // Guardar nuevo riego
        void saveLog(uint8_t valve, uint16_t liters, uint32_t startT, uint32_t endT);
        void saveError(uint8_t errorCode, uint16_t detail);
        // Recuperar por índice relativo (0 es el más antiguo, total-1 el más nuevo)
        LogEntry getLog(uint16_t logicalIndex);
    
        uint16_t getCount(); // Cuántos registros válidos hay realmente
        void clearMemory();
        void dumpLogsToSerial();   

        // Gestión de Litros Totales (Vida del aparato) en EEPROM Interna
        uint32_t getTotalLifetimeLiters();
        void addLifetimeLiters(uint16_t liters);
        void resetLifetimeLiters();

        void adjustTime(const DateTime& dt) { RTC.adjust(dt); }
        DateTime now() { return RTC.now(); }
};
#endif
