//Program.h
#ifndef Program_h
#define Program_h
#include <RTClib.h>

#include "Frame.h"

class Program {
    private:
        uint8_t hour;
        uint8_t duration;
        uint8_t days[7];
        Frame* frame;
        
    public:
        Program(uint8_t Hour,uint8_t Duration,const uint8_t* Days, Frame* FRame );
        int setProgram(uint8_t Hour,uint8_t Duration,const uint8_t* Days, Frame* FRame );
        uint8_t getHour();
        uint8_t getDuration();
        const uint8_t* getDays();
        Frame* getFrame();
        bool inTimeFrame(DateTime date);
};
#endif
