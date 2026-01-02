//Clock.h
#ifndef Clock_h
#define Clock_h
#include <Arduino.h>
#include <RTClib.h>

class Clock {
    private:
        RTC_DS3231 RTC;
        const char D[7][3] = {"DG","DL","DT","DC","DJ","DV","DS"};
        const char E[12][4] = {"HIV","HIV","HIV","PRI","PRI","PRI","EST","EST","EST","TAR","TAR","TAR"};

    public:
        Clock();
        void init();
        DateTime now();
        char* getHour();
        const char* getDayOfTheWeek();
        const char* getStacion();
        
};
#endif
