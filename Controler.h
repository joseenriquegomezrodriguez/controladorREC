//Controler.h
#ifndef Controler_h
#define Controler_h
#include <Arduino.h>
#include "Electrovalve.h"
#include "Pumb.h"
#include "Button.h"
#include "Display.h"
#include "Clock.h"

class Controler{
    private:
    uint8_t state;
    Electrovalve* E1;
    Electrovalve* E2;
    Electrovalve* E3;
    Pumb* pumb;
    Button* ModeButton;
    Button* E1Button;
    Button* E2Button;
    Button* E3Button;
    Display display;
    Clock clock;
    DateTime backLightTime;
    uint8_t backLightDuration;
    
    public:
        Controler(Electrovalve* e1,Electrovalve* e2,Electrovalve* e3, Pumb* PUmb, Button* modeButton, Button* e1Button, Button* e4Button, Button* e3Button);
        void checkIrrigation();
        void changeState();
        void setAuto();
        void setManual();
        void setStop();
        void setBackLightTime(DateTime time);
        void checkButtons();
        void checkBackLight();
        void check();
        void init();

        

};
#endif
