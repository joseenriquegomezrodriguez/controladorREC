//Program.cpp
#include "Program.h"
#include <RTClib.h>
#include <Arduino.h>


Program::Program(uint8_t Hour,uint8_t Duration, const uint8_t* Days, Frame* FRame ) : hour(Hour),duration(Duration), frame(FRame) {
    for (int i = 0; i < 7; ++i) {
        days[i] = Days[i]; 
    }
};

int Program::setProgram(uint8_t Hour,uint8_t Duration, const uint8_t* Days, Frame* FRame )  {
    hour = Hour;
    duration = Duration;
    frame = FRame;
    for (int i = 0; i < 7; ++i) {
        days[i] = Days[i]; 
    }
    return 0;
};
uint8_t Program::getHour(){
    return hour;
};
uint8_t Program::getDuration(){
    return duration;
};
const uint8_t* Program::getDays(){
    return days;
};
Frame* Program::getFrame(){
    return frame;
};
bool Program::inTimeFrame(DateTime date){
    return false;
};
