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
    float _flowRate; // Caudal instantáneo acumulado
    
    static FlowSensor* _instance; // Puntero para la interrupción

  public:
    // Constructor
    FlowSensor(uint8_t pin, float kFactor = 7.5);
    ~FlowSensor();

    
    static void handleInterrupt();// Manejador de interrupción (estático) - Público para permitir simulación en tests

    void init();
    void reset();          // Pone el contador a cero para un nuevo riego
    void setPulses(uint32_t pulses); // Inyectar pulsos (útil para tests o restauración)
    float getVolume();     // Devuelve los litros acumulados
    uint32_t getPulses();  // Devuelve los pulsos totales (por si acaso)
    float getInstantFlow(); // Nueva función: Devuelve Litros por Minuto (L/min)
    float getKFactor() const { return _kFactor; }
};

#endif