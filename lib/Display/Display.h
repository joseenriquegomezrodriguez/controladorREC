//Display.h
#ifndef Display_h
#define Display_h
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class Display {
    private:
        LiquidCrystal_I2C lcd;
        bool backLight;
        // Posiciones de las variables en pantalla
        const uint8_t pos_E[2]  = {17, 0};
        const uint8_t pos_M[2]  = {7, 1};
        const uint8_t pos_EV[2] = {12, 1};
        
        const uint8_t pos_VA[2] = {11, 2};
        const uint8_t pos_D[2]  = {9, 3};
        const uint8_t pos_H[2]  = {14, 3}; // Hora
        const uint8_t pos_HT[2] = {3, 2};  // Humedad del suelo (Soil Moisture)
        void print(uint8_t X, uint8_t Y,const char* txt);
       
    public:
        Display();
        void init();
        void setON();
        void setOFF();
        bool getBackLight();
        void printHour(const char* txt);
        void printStation(const char* txt);
        void printSoilMoisture(int soilMoisture);
                
        void printDay(const char* txt);
        void printMode(const char* txt);
        void printValveStatus(uint8_t index, char* text);
        void printFlow(float flow);
};
#endif
