//Controler.cpp
#include "Controler.h"
#include "Electrovalve.h"

static char printBuffer[3];

Controler::Controler(Electrovalve* e1,Electrovalve* e2,Electrovalve* e3,  Pumb* PUmb, Button* modeButton, Button* e1Button, Button* e2Button, Button* e3Button):E1(e1),E2(e2),E3(e3),pumb(PUmb),ModeButton(modeButton),E1Button(e1Button),E2Button(E2Button),E3Button(e3Button),state(2),backLightDuration(2){
};
void Controler::setAuto(){
  state = 0;

};
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
    sprintf(printBuffer, "%s1", E1->getLabelState());
    display.print(pos_E1[0], pos_E1[1], printBuffer);
  };
  if ((E2Button->read()==LOW)&&(display.getBackLight())){
    E2->changeState();
    sprintf(printBuffer, "%s2", E2->getLabelState());
    display.print(pos_E2[0], pos_E2[1], printBuffer);
  };
  if ((E3Button->read()==LOW)&&(display.getBackLight())){
    E3->changeState();
    sprintf(printBuffer, "%s3", E3->getLabelState());
    display.print(pos_E3[0], pos_E3[1], printBuffer);
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
  display.print(pos_H[0],pos_H[1],clock.getHour());

  //Print dayOfTheWeek
  display.print(pos_D[0],pos_D[1],clock.getDayOfTheWeek());
  
  
  Controler::checkButtons();
  Controler::checkBackLight();
  Controler::checkIrrigation();
};
