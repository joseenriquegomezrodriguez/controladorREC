//Controler.h
#ifndef Controler_h
#define Controler_h
#include <Arduino.h>
class Electrovalve;

class Controler{
    private:
    Electrovalve& E1;
    Electrovalve& E2;
    Electrovalve& E3;
    
    public:
        Controler(Electrovalve& e1,Electrovalve& e2,Electrovalve& e3);

};
#endif