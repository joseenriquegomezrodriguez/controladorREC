//THSensor.cpp

#include "THSensor.h"
#include <DHT.h>
#include <Wire.h>
#include <Arduino.h>

THSensor::THSensor(DHT& Dht):dht(Dht){
  
}


int THSensor::readHumidity(){
  return (int)dht.readHumidity();
}

int THSensor::readTemperature(){
  return (int)dht.readTemperature();
}


void THSensor::init(){
  
  dht.begin();
}