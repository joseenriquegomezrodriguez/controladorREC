#ifndef FLOW_SENSOR_H
#define FLOW_SENSOR_H

#include <Arduino.h>

class FlowSensor {
  private:
    uint8_t _pin;
    float _kFactor; // Factor de conversión (ej: 7.5)
    volatile uint32_t _pulseCount;

    // Variables para el cálculo de caudal instantáneo
    uint32_t _lastPulseCount; 
    uint32_t _lastTime;
    
    static FlowSensor* _instance; // Puntero para la interrupción

    // Manejador de interrupción (estático)
    static void handleInterrupt();

  public:
    // Constructor
    FlowSensor(uint8_t pin, float kFactor = 7.5);

    void begin();
    void reset();          // Pone el contador a cero para un nuevo riego
    float getVolume();     // Devuelve los litros acumulados
    uint32_t getPulses();  // Devuelve los pulsos totales (por si acaso)
    // Nueva función: Devuelve Litros por Minuto (L/min)
    float getInstantFlow();
};

#endif