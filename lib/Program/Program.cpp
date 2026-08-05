//Program.cpp
#include <Arduino.h>
#include "Program.h"
#include <RTClib.h>



Program::Program(uint8_t Hour,uint8_t Minute, uint8_t Duration, const uint8_t* Days, Frame* FRame ) : hour(Hour), minute(Minute), duration(Duration), frame(FRame) {
    for (int i = 0; i < 7; ++i) {
        days[i] = Days[i]; 
    }
};

int Program::setProgram(uint8_t Hour,uint8_t Minute ,uint8_t Duration, const uint8_t* Days, Frame* FRame )  {
    hour = Hour;
    minute = Minute;
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
uint8_t Program::getMinute(){
    return minute;
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
    if (frame != nullptr && !frame->inTimeFrame(date)) return false;

    uint8_t dayOfWeek = date.dayOfTheWeek(); // 0 = Sunday, 1 = Monday, ..., 6 = Saturday
    uint8_t currentHour = date.hour();
    uint8_t currentMinute = date.minute();

    if (days[dayOfWeek] != 1) return false;

    // Calculamos minutos totales desde las 00:00 para manejar duraciones largas
    uint16_t startTotalMinutes = (uint16_t)hour * 60 + minute;
    uint16_t currentTotalMinutes = (uint16_t)currentHour * 60 + currentMinute;
    uint16_t endTotalMinutes = startTotalMinutes + duration;

    return (currentTotalMinutes >= startTotalMinutes && currentTotalMinutes < endTotalMinutes);
};
