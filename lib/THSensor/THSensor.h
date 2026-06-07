//THSensor.h
#ifndef THSensor_h
#define THSensor_h


#include <Arduino.h>
#include <DHT.h>
#include <Wire.h>


class THSensor;

class THSensor {
  private:
    DHT& dht;
    
  public:
    THSensor(DHT& Dht); 
    int readTemperature();
    int readHumidity();
    void init();
};
#endif
