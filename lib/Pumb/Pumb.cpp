//Pumb.cpp
#include <Arduino.h>
#include "Pumb.h"
#include "Rele.h"
#include <RTClib.h>


Pumb::Pumb(uint8_t relePin) : Rele(relePin, false) {
    // No llamar a init() aquí, dejar que Controller o Main lo hagan
};
