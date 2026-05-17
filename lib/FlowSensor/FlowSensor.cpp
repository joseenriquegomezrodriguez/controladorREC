#include <Arduino.h>
#include "FlowSensor.h"

// Inicializamos el puntero estático a nulo
FlowSensor* FlowSensor::_instance = nullptr;

FlowSensor::FlowSensor(uint8_t pin, float kFactor) {
  _pin = pin;
  _kFactor = kFactor;
  _pulseCount = 0;
  _instance = this;
}

void FlowSensor::begin() {
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
  uint32_t currentPulses;
  
  noInterrupts();
  currentPulses = _pulseCount;
  interrupts();

  // Calcular tiempo transcurrido en segundos (usando micros para más precisión)
  float duration = (currentTime - _lastTime) / 1000000.0;
  if (duration <= 0) return 0;

  // Calcular pulsos en este intervalo
  uint32_t pulsesInInterval = currentPulses - _lastPulseCount;
  
  // Frecuencia (Hz) = pulsos / segundos
  float frequency = pulsesInInterval / duration;
  
  // Caudal (L/min) = Frecuencia / K
  float flowRate = frequency / _kFactor;

  // Guardar estado para la próxima lectura
  _lastPulseCount = currentPulses;
  _lastTime = currentTime;

  return flowRate;
}

void FlowSensor::reset() {
  noInterrupts(); // Protección para evitar que cambie mientras reseteamos
  _pulseCount = 0;
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
