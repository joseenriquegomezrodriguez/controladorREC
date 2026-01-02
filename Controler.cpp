//Controler.cpp
#include "Controler.h"
#include "Electrovalve.h"

static char printBuffer[3];

Controler::Controler(Electrovalve* e1,Electrovalve* e2,Electrovalve* e3,  Pumb* PUmb, Button* modeButton, Button* e1Button, Button* e2Button, Button* e3Button):E1(e1),E2(e2),E3(e3),pumb(PUmb),ModeButton(modeButton),E1Button(e1Button),E2Button(E2Button),E3Button(e3Button),state(2),backLightDuration(2){
};
void Controler::setAuto(){
  state = 0;
  display.printMode("AUTO");
};
void Controler::setManual(){
    state = 1;
    pumb->setON();
    display.printMode("MANU");
};
void Controler::setStop(){
    state = 2;
    pumb->setOFF();
    display.printMode("STOP");
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
    sprintf(printBuffer, "%s1", E1->getLabelState());
    display.printE1(printBuffer);
  };
  if ((E2Button->read()==LOW)&&(display.getBackLight())){
    E2->changeState();
    sprintf(printBuffer, "%s2", E2->getLabelState());
    display.printE2(printBuffer);
  };
  if ((E3Button->read()==LOW)&&(display.getBackLight())){
    E3->changeState();
    sprintf(printBuffer, "%s3", E3->getLabelState());
    display.printE3(printBuffer);
  };
  if ((E1Button->read()==LOW)&&(E1Button->read()==LOW)&&(E1Button->read()==LOW)&&(!display.getBackLight())){
    display.setON();
    setBackLightTime(clock.now());
  };
};

void Controler::checkIrrigation(){
  
};
void Controler::check(){

  //Print hour
  display.printHour(clock.getHour());

  //Print dayOfTheWeek
  display.printDay(clock.getDayOfTheWeek());
  
  //Print estation
  display.printStation(clock.getStacion());
  
  Controler::checkButtons();
  Controler::checkBackLight();
  Controler::checkIrrigation();
};
