//Controler.h
#ifndef Controler_h
#define Controler_h
#include <Arduino.h>
#include "Electrovalve.h"
#include "Pumb.h"
#include "Button.h"
#include "FlowSensor.h"
#include "Display.h"
#include "Clock.h"
class Controler{
    private:
    uint8_t state;

    Electrovalve** valves;      // Array de punteros a electroválvulas
    Button** valveButtons;      // Array de punteros a sus botones
    uint8_t numValves;          // Número total de válvulas
    //Electrovalve* E1;
    //Electrovalve* E2;
    //Electrovalve* E3;
    Pumb* pumb;
    FlowSensor* flowSensor;
    Button* ModeButton;
    //Button* E1Button;
    //Button* E2Button;
    //Button* E3Button;
    Display display;
    Clock clock;
    DateTime backLightTime;
    uint8_t backLightDuration;

    
    
    
    public:
        Controler(Electrovalve** v_array, Button** b_array, uint8_t n_valves, Pumb* PUmb, FlowSensor* flowSensor, Button* modeButton);
        void checkIrrigation();
        void changeState();
        void setAuto();
        void setManual();
        void setStop();
        void setBackLightTime(DateTime time);
        void checkButtons();
        void checkBackLight();
        void check();
        void securityCheck();
        void init();

        

};
#endif
