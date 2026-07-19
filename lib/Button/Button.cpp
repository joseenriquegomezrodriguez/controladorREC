//Button.cpp
#include <Arduino.h>
#include "Button.h"

Button::Button(uint8_t Pin) : pin(Pin){

}

void Button::init(){
    // Configuramos el pin para usar la resistencia pull-up interna del Arduino
    pinMode(pin, INPUT_PULLUP);
    lastState = HIGH;
};
int Button::read(){
    return digitalRead(pin);
}
void Button::setLastState(bool state){
    lastState = state;
}
bool Button::getLastState(){
    return lastState;
}
