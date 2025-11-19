//Controler.cpp
#include "Controler.h"
#include "Electrovalve.h"


Controler::Controler(Electrovalve* e1,Electrovalve* e2,Electrovalve* e3,  Pumb* PUmb, Button* modeButton, Button* e1Button, Button* e2Button, Button* e3Button):E1(e1),E2(e2),E3(e3),pumb(PUmb),ModeButton(modeButton),E1Button(e1Button),E2Button(E2Button),E3Button(e3Button),state(2),backLightDuration(2){
};
void Controler::setAuto(){state = 0;};
void Controler::setManual(){
    state = 1;
    pumb->setON();
};
void Controler::setStop(){
    state = 2;
    pumb->setOFF();
};

void Controler::init(){};

void Controler::checkBackLight(){
if (display.getBackLight()){
    DateTime today = clock.now();
    int nowMinutes = today.hour() * 60 + today.minute();
    int backLightMinuts = backLightTime.hour() * 60 + backLightTime.minute();
    if (!((nowMinutes >= backLightMinuts) && (nowMinutes < (backLightMinuts + backLightDuration)))){
     display.setOFF();
    };
  };
};

void Controler::changeState(){
  switch (state) {
    case 0: {
        setManual();
        return;
      }
      case 1: {
        setStop();
        return;
      }
      case 2: {
        setAuto();
        return;
      }
  };
};
void Controler::setBackLightTime(DateTime time){
  Controler::setBackLightTime(time);
  };

void Controler::checkButtons(){
  if ((E1Button->read()==LOW)&&(display.getBackLight())){
    E1->changeState();
  };
  if ((E2Button->read()==LOW)&&(display.getBackLight())){
    E2->changeState();
  };
  if ((E3Button->read()==LOW)&&(display.getBackLight())){
    E3->changeState();
  };
  if ((E1Button->read()==LOW)&&(E1Button->read()==LOW)&&(E1Button->read()==LOW)&&(!display.getBackLight())){
    display.setON();
    setBackLightTime(clock.now());
  };
};

void Controler::checkIrrigation(){
  
};
void Controler::check(){
  Controler::checkButtons();
  Controler::checkBackLight();
  Controler::checkIrrigation();
};
