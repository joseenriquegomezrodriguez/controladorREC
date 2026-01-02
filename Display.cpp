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
    lcd.setCursor(0,0);
    lcd.print("CONTROL DE REG E:xxx");
    lcd.setCursor(0,1);
    lcd.print("T:xx M:xxxx E1 E2 E3");
    lcd.setCursor(0,2);
    lcd.print("HT:xxxx VA:xx.xL/min");
    lcd.setCursor(0,3);
    lcd.print("HA:xx% D:xx H:");

};
bool Display::getBackLight(){
    return backLight;
};
void Display::print(uint8_t X, uint8_t Y, const char* txt ){
    lcd.setCursor(X,Y);
    lcd.print(txt);
};
void Display::printE1(const char* txt){
    print(pos_E1[0], pos_E1[1], txt);
};
void Display::printE2(const char* txt){
    print(pos_E2[0], pos_E2[1], txt);
};
void Display::printE3(const char* txt){
    print(pos_E3[0], pos_E3[1], txt);
};
void Display::printHour(const char* txt){
    print(pos_H[0],pos_H[1],txt);
};
void Display::printDay(const char* txt){
    print(pos_D[0],pos_D[1],txt);
};
void Display::printStation(const char* txt){
    print(pos_E[0],pos_E[1],txt);
};
void Display::printMode(const char* txt){
    print(pos_M[0],pos_M[1],txt);
};