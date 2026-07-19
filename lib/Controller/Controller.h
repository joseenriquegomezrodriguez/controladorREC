#ifndef CONTROLLER_H
#define CONTROLLER_H

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
extern FlowSensor flowSensor;

class Controller {
private:
    Electrovalve** valves;
    Button** valveButtons;
    uint8_t numValves;
    Pumb* pumb;
    Button* ModeButton;
    CustomKeypad* keypad;
    bool useKeypad;
    SoilSensor* soilSensor;
    THSensor* thSensor;
    
    uint8_t state; // 0: STOP, 1: AUTO, 2: MANU
    uint8_t backLightDuration;
    DateTime backLightTime;
    bool moistureInhibit;

    // Tracking variables for state machine
    uint32_t leakStartTime; // Time when leak was detected
    uint32_t leakStartPulses; // Pulses when leak was detected
    uint32_t manualStartTime; // Time when manual mode started
    uint32_t manualStartPulses; // Pulses when manual mode started
    uint32_t noFlowStartTime; // Time when no flow was detected in manual mode
    uint32_t lastActiveTime; // Time when the last irrigation activity occurred
    

    void checkKeypad();
    void checkButtons();
    void checkBackLight();
    void stopController();
    void autoController(DateTime today, float correctionFactor);
    void manualController();
    void checkAndSaveActiveLeak(DateTime today);
    void checkCmds();
    void printHelp();

    float getCorrectionFactor();
 
    void setAuto();
    void setManual();
    void setStop();
    void changeState();
    
    void setBackLightTime(DateTime time);
public:
    Controller(Electrovalve** v_array, Button** b_array, uint8_t n_valves, 
              Pumb* PUmb, Button* modeButton, SoilSensor* soilSensor, THSensor* thSensor);
    Controller(Electrovalve** v_array, uint8_t n_valves, 
              Pumb* PUmb, CustomKeypad* keypad, SoilSensor* soilSensor, THSensor* thSensor);
    
    void init();
    void check();
    
    
};

#endif