//Frame.cpp

#include "Frame.h"
#include <RTClib.h>
#include <Arduino.h>


Frame::Frame(DateTime Init, DateTime End) {
  
  init = Init;
  end = End;
 
}
void Frame::setFrame(DateTime Init, DateTime End) {
  init = Init;
  end = End;
}
DateTime Frame::getInit(){
    return init;
}

DateTime Frame::getEnd(){
    return end;
}

bool Frame::inTimeFrame(DateTime date){
    if((date > init)&&(date < end)) return true;
    else return false;
}
