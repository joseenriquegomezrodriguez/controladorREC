//Pumb.cpp
#include "Pumb.h"
#include "Rele.h"
#include <RTClib.h>


Pumb::Pumb( uint8_t relePin) :  Rele(relePin,true){
  
init();
setOFF();
 
};
