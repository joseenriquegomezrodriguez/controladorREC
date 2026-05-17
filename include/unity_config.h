#ifndef UNITY_CONFIG_H
#define UNITY_CONFIG_H

// Define UNITY_INCLUDE_ARDUINO_API para incluir las funciones de Arduino en Unity
#define UNITY_INCLUDE_ARDUINO_API

// Define UNITY_PRINT_EOL para usar Serial.println() como fin de línea
#define UNITY_PRINT_EOL() { UNITY_OUTPUT_CHAR('\r'); UNITY_OUTPUT_CHAR('\n'); }

#endif // UNITY_CONFIG_H