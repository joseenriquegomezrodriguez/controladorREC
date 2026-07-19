#include <Arduino.h>
#include "FlowSensor.h"

// Inicializamos el puntero estático a nulo
FlowSensor* FlowSensor::_instance = nullptr;

FlowSensor::FlowSensor(uint8_t pin, float kFactor) {
  _pin = pin;
  _kFactor = kFactor;
  _pulseCount = 0;
  _lastPulseCount = 0;
  _lastTime = 0;
  _flowRate = 0.0f;
  _instance = this;
}

FlowSensor::~FlowSensor() {
  if (_instance == this) {
    _instance = nullptr;
  }
}

void FlowSensor::init() {
  pinMode(_pin, INPUT_PULLUP);
  // Configuramos la interrupción en el flanco de subida (RISING)
  attachInterrupt(digitalPinToInterrupt(_pin), FlowSensor::handleInterrupt, RISING);
}

void FlowSensor::handleInterrupt() {
  if (_instance != nullptr) {
    _instance->_pulseCount++;
  }
}
float FlowSensor::getInstantFlow() {
  uint32_t currentTime = micros();
  
  // Solo recalculamos el caudal si han pasado al menos 100 ms (100,000 micros)
  // Esto permite llamar a la función desde múltiples sitios del bucle (display,
  // control de fugas, etc.) sin vaciar los pulsos acumulados en lecturas intermedias rápidas.
  if (currentTime - _lastTime >= 100000) {
    uint32_t currentPulses;
    noInterrupts();
    currentPulses = _pulseCount;
    interrupts();

    float duration = (currentTime - _lastTime) / 1000000.0;
    if (duration > 0) {
      uint32_t pulsesInInterval = currentPulses - _lastPulseCount;
      float frequency = pulsesInInterval / duration;
      _flowRate = frequency / _kFactor;
      
      _lastPulseCount = currentPulses;
      _lastTime = currentTime;
    }
  }
  return _flowRate;
}

void FlowSensor::reset() {
  noInterrupts(); // Protección para evitar que cambie mientras reseteamos
  _pulseCount = 0;
  interrupts();
}

void FlowSensor::setPulses(uint32_t pulses) {
  noInterrupts();
  _pulseCount = pulses;
  interrupts();
}

uint32_t FlowSensor::getPulses() {
  uint32_t count;
  noInterrupts();
  count = _pulseCount;
  interrupts();
  return count;
}

float FlowSensor::getVolume() {
  // Según la mayoría de sensores (YF-S201): 
  // Frecuencia (Hz) = 7.5 * Q (L/min)
  // 1 Litro = 450 pulsos (aproximadamente, si K=7.5)
  // Volumen = pulsos / (K * 60)
  return (float)getPulses() / (_kFactor * 60.0);
}
