//Controler.cpp
#include "Controler.h"
#include "Electrovalve.h"


Controler::Controler(Electrovalve** v_array, Button** b_array, uint8_t n_valves,  Pumb* PUmb, FlowSensor* flowSensor, Button* modeButton):valves(v_array), valveButtons(b_array), numValves(n_valves), pumb(PUmb), flowSensor(flowSensor), ModeButton(modeButton), state(2), backLightDuration(2){
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
  this->setBackLightTime(time);
  };

void Controler::checkButtons(){
  bool anyButtonPressed = false;
    char printBuffer[20]; // Buffer para sprintf

    // 1. Recorremos todas las válvulas con un bucle
    for (uint8_t i = 0; i < numValves; i++) {
        
        // Comprobamos si el botón de la válvula 'i' está presionado
        if (valveButtons[i]->read() == LOW) {
            anyButtonPressed = true;

            // Si la luz está encendida, ejecutamos la acción de la válvula
            if (display.getBackLight()) {
                valves[i]->changeState();

                // Armamos el texto (ej: "ON 1", "OFF 2", etc.)
                // Usamos i + 1 para que el usuario vea "Válvula 1" en vez de "Válvula 0"
                sprintf(printBuffer, "%s%d", valves[i]->getLabelState(), i + 1);

                // ACTUALIZACIÓN DEL DISPLAY
                // Aquí llamamos a una función genérica de tu clase Display
                display.printValveStatus(i, printBuffer); 
            }
        }
    }

    // 2. Lógica del Backlight (Si se presiona CUALQUIER botón con la luz apagada)
    if (anyButtonPressed && !display.getBackLight()) {
        display.setON();
        setBackLightTime(clock.now());
        
        // Opcional: Pequeño delay para evitar que el primer toque 
        // también active la válvula accidentalmente
        delay(200); 
    }
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
  
  this->checkButtons();
  this->checkBackLight();
  this->checkIrrigation();
};
