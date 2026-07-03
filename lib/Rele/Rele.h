//Rele.h
#ifndef Rele_h
#define Rele_h
#include <Arduino.h>
class Rele {
    protected:
        bool status;
        bool _inverted;
        uint8_t pin;

        // Campos compartidos para el seguimiento de la actividad (Riego/Bomba)
        uint32_t _startTime;
        uint32_t _startPulses;
        bool _isManualStart;

    public:
        Rele(uint8_t Pin,bool Status);
        virtual void init();
        virtual void setON();
        virtual void setOFF();
        bool getStatus(); // Mantenido por compatibilidad
        bool isActive() const { return status; }

        // Métodos de acceso para el seguimiento de consumo y tiempo
        uint32_t getStartTime() const;
        void setStartTime(uint32_t time);
        uint32_t getStartPulses() const;
        void setStartPulses(uint32_t pulses);
        bool getIsManualStart() const;
        void setIsManualStart(bool manual);
};
#endif
