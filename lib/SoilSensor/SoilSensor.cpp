#include "SoilSensor.h"

SoilSensor::SoilSensor(const int SensorPin) : sensorPin(SensorPin) {}

void SoilSensor::init() {
    pinMode(sensorPin, INPUT);
}

int SoilSensor::getMoisturePercentage() {
    int rawValue = analogRead(sensorPin);
    
    // Map the raw analog value to a percentage (0-100%)
    // dryValue (600) -> 0%
    // wetValue (250) -> 100%
    float percentage = map(rawValue, dryValue, wetValue, 0, 100);
    
    // Constrain the value between 0 and 100
    if (percentage < 0) percentage = 0;
    if (percentage > 100) percentage = 100;
    
    return percentage;
}
int SoilSensor::getDryValue() {
    return dryValue;
}
int SoilSensor::getWetValue() {
    return wetValue;
} 
void SoilSensor::calibrateDry(int value) {
    // Logic to capture current reading as dryValue could be implemented here
    // For now, it uses the hardcoded constant
    dryValue = value;
}

void SoilSensor::calibrateWet(int value) {
    // Logic to capture current reading as wetValue could be implemented here
    // For now, it uses the hardcoded constant
    wetValue = value;
}
