//Frame.h
#ifndef Frame_h
#define Frame_h

#include <RTClib.h>
#include <Arduino.h>

class Frame {
    private:
        DateTime init;
        DateTime end;
    public:
        Frame(DateTime Init, DateTime End);
        void setFrame(DateTime Init, DateTime End);
        DateTime getInit();
        DateTime getEnd();
        bool inTimeFrame(DateTime date);
};
#endif
