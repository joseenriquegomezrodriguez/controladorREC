#include "CustomKeypad.h"

CustomKeypad::CustomKeypad(uint8_t address) : _keypad(address), _lastKey('\0') {
}

void CustomKeypad::init() {
    Wire.begin(); 
    if (!_keypad.begin()) {
        Serial.println(F("Error: No se encuentra el teclado I2C en la direccion especificada."));
    }
}

char CustomKeypad::getKey() {
    // La librería devuelve un índice 0..15 si hay tecla, o 16 si no hay ninguna.
    uint8_t index = _keypad.getKey();
    char currentKey = (index < 16) ? _layout[index] : '\0';

    // Lógica para detectar solo el flanco de bajada (una sola vez por pulsación)
    if (currentKey != _lastKey) {
        _lastKey = currentKey;
        // Si la tecla actual no es nula, es que acabamos de presionar algo
        if (currentKey != '\0') {
            return currentKey;
        }
    }
    
    return '\0';
}