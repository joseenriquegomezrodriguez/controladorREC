//Clock.cpp
#include "Clock.h"

Clock::Clock(){
    init();
};
void Clock::init(){
  Serial.println(__DATE__);
  Serial.println(__TIME__);
  if (!RTC.begin()){
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }else{  
    Serial.println("Find RTC");
     // RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  if (RTC.lostPower()) {
    // Solo ajustar si se perdió la hora
    RTC.adjust(DateTime(__DATE__, __TIME__));
  };
};

DateTime Clock::now(){
    return RTC.now();
};
