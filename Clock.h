//Clock.h
#ifndef Clock_h
#define Clock_h
#include <Arduino.h>
#include <RTClib.h>

class Clock {
    private:
        RTC_DS3231 RTC;
    public:
        Clock();
        void init();
        DateTime now();
        
};
#endif
