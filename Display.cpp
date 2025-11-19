//Display.cpp
#include "Display.h"

Display::Display(): lcd(0x27, 20, 4){
    
    Display::init();
};
void Display::setON(){
    lcd.backlight();
    backLight = true;
};
void Display::setOFF(){
    lcd.noBacklight();
    backLight = false;
};
void Display::init(){
    Display::setON();
};
bool Display::getBackLight(){
    return backLight;
};
void Display::print(uint8_t X, uint8_t Y, char16_t txt ){
    lcd.setCursor(X,Y);
    lcd.print(txt);
};
