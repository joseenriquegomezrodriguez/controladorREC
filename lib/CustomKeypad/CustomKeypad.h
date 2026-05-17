#ifndef CUSTOM_KEYPAD_H
#define CUSTOM_KEYPAD_H

#include <Arduino.h>
#include <Wire.h>
#include <I2CKeyPad.h>

class CustomKeypad {
private:
    I2CKeyPad _keypad;
    // Mapa de teclas para un teclado 4x4 estándar. 
    // El índice 0..15 depende de cómo esté cableado el teclado al PCF8574.
    char _layout[17] = "123A456B789C*0#D"; 
    char _lastKey;

public:
    CustomKeypad(uint8_t address);
    void init();
    char getKey();
};

#endif