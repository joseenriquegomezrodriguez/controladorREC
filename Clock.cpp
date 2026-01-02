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

char* Clock::getHour(){
  
  static char hourBuffer[6]; // 'static' ensures the memory persists after function ends
  DateTime today = now();
  sprintf(hourBuffer, "%02d:%02d", today.hour(), today.minute());
  return hourBuffer;
};

const char* Clock::getDayOfTheWeek(){
  DateTime today = now();
  return D[today.dayOfTheWeek()];
};
const char* Clock::getStacion(){
  DateTime today = now();
  return E[today.month()-1];
};
