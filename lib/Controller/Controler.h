#ifndef CONTROLER_H
#define CONTROLER_H

#include <Arduino.h>
#include "Electrovalve.h"
#include "Button.h"
#include "Pumb.h"
#include "FlowSensor.h"
#include "Clock.h"
#include "Display.h"
#include "CustomKeypad.h"
#include "SoilSensor.h"
#include "THSensor.h"

// Referencias a objetos globales definidos en el main
extern Display display;
extern Clock clock;

class Controler {
private:
    Electrovalve** valves;
    Button** valveButtons;
    uint8_t numValves;
    Pumb* pumb;
    FlowSensor* flowSensor;
    Button* ModeButton;
    CustomKeypad* keypad;
    bool useKeypad;
    SoilSensor* soilSensor;
    THSensor* thSensor;
    
    uint8_t state; // 0: AUTO, 1: MANU, 2: STOP
    uint8_t backLightDuration;
    DateTime backLightTime;

    void checkKeypad();
    void checkButtons();
    void checkBackLight();
    void checkIrrigation(DateTime today);
    void checkCmds();
    void printHelp();

public:
    Controler(Electrovalve** v_array, Button** b_array, uint8_t n_valves, 
              Pumb* PUmb, FlowSensor* flowSensor, Button* modeButton, SoilSensor* soilSensor, THSensor* thSensor);
    Controler(Electrovalve** v_array, uint8_t n_valves, 
              Pumb* PUmb, FlowSensor* flowSensor, CustomKeypad* keypad, SoilSensor* soilSensor, THSensor* thSensor);
    
    void init();
    void check();
    
    void setAuto();
    void setManual();
    void setStop();
    void changeState();
    
    void setBackLightTime(DateTime time);
};

#endif