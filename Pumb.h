//Pumb.h
#ifndef Pumb_h
#define Pumb_h
#include <Arduino.h>
#include "Rele.h"

class Pumb : public Rele{
    public:
        Pumb(uint8_t relePin);
};
#endif
