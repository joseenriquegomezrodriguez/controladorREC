#include <Arduino.h>

#include <unity.h>
#include "FlowSensor.h"

// Instancia para pruebas. Usamos el pin 2 y K=7.5
FlowSensor* sensor;

// Se ejecuta antes de cada test
void setUp(void) {
    sensor = new FlowSensor(2, 7.5);
}

// Se ejecuta después de cada test
void tearDown(void) {
    delete sensor;
}

void test_sensor_initialization(void) {
    TEST_ASSERT_EQUAL_FLOAT(0.0, sensor->getVolume());
    TEST_ASSERT_EQUAL_UINT32(0, sensor->getPulses());
}

void test_volume_calculation(void) {
    sensor->reset();
    
    // Para K = 7.5, la fórmula es: Volumen = pulsos / (K * 60)
    // Para obtener 1.0 Litro: pulsos = 1.0 * 7.5 * 60 = 450 pulsos
    for (int i = 0; i < 450; i++) {
        FlowSensor::handleInterrupt();
    }

    TEST_ASSERT_EQUAL_FLOAT(1.0, sensor->getVolume());
    TEST_ASSERT_EQUAL_UINT32(450, sensor->getPulses());
}

void test_instant_flow_calculation(void) {
    sensor->reset();

    // 1. Inicializar tiempo interno
    sensor->getInstantFlow();
    
    // 2. Simular el paso de 1 segundo real en la placa
    delay(1000); 

    // 3. Simular 75 pulsos
    // Si K = 7.5 y Freq = 75Hz -> Caudal = 75 / 7.5 = 10.0 L/min
    for (int i = 0; i < 75; i++) {
        FlowSensor::handleInterrupt();
    }

    float flow = sensor->getInstantFlow();
    // Usamos WITHIN porque delay(1000) no es exactamente 1s a nivel de microsegundos
    TEST_ASSERT_FLOAT_WITHIN(0.5, 10.0, flow);
}

void run_all_tests() {
    UNITY_BEGIN();
    RUN_TEST(test_sensor_initialization);
    RUN_TEST(test_volume_calculation);
    RUN_TEST(test_instant_flow_calculation);
    
    UNITY_END();
}

void setup() {
    // Tiempo para que el monitor serie se estabilice
    delay(2000);
    run_all_tests();
}

void loop() {}