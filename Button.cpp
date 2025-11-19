//Button.cpp
#include "Button.h"

Button::Button(uint8_t Pin) : pin(Pin){

}

void Button::init(){
    pinMode(pin, INPUT_PULLUP);
};
int Button::read(){
    return digitalRead(pin);
}
