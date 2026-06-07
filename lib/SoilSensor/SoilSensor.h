#ifndef SoilSensor_h
#define SoilSensor_h
#include <Arduino.h>



class SoilSensor;

class SoilSensor {
  private:
    const int sensorPin;
    int dryValue = 600;  // Example: Sensor reading in air
    int wetValue = 250;
    
    
  public:
    SoilSensor(const int SensorPin); 
    void init();
    int read();
    void calibrateDry(int value);
    void calibrateWet(int value);
};

#endif
