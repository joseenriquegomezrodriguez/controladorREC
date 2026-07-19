#include <Wire.h>
#include <Arduino.h>
#include <RTClib.h> // Necesario para DateTime
#include <DHT.h>
#include "Controller.h"
#include "Display.h"
#include "Clock.h"
#include "Frame.h"
#include "Electrovalve.h"
#include "Program.h"
#include "Pumb.h"
#include "FlowSensor.h"
#include "Button.h"
#include "SoilSensor.h"
#include "THSensor.h"
#include "CustomKeypad.h"


Display display;
Clock clock;
SoilSensor soilSensor(A0); // Sensor de humedad conectado al pin A0
DHT dht(3, DHT11); 
THSensor thSensor(dht); // Sensor de temperatura y humedad conectado al pin 3

// Definiciones de arrays de días fijos (necesario si Program espera un const uint8_t days[])
const uint8_t NO_DAYS[7] = {0, 0, 0, 0, 0, 0, 0};
const uint8_t DAILY[7]   = {1, 1, 1, 1, 1, 1, 1}; 
const uint8_t SATURDAY[7] = {0, 0, 0, 0, 0, 0, 1};
const uint8_t MON_WED_SAT[7] = {0, 1, 0, 1, 0, 0, 1}; 

  // 1. CORRECCIÓN: Inicialización de objetos temporales DateTime
  // Se usa la sintaxis estándar de constructor C++: Tipo(param1, param2...)
  // y se elimina 'new' para objetos stack.
  
  // Definición de estaciones para el año 2026
  Frame winter(DateTime(2025, 12, 22), DateTime(2026, 3, 20, 23, 59, 59)); // Invierno actual
  Frame spring(DateTime(2026, 3, 21), DateTime(2026, 6, 21, 23, 59, 59));  // Primavera 2026
  Frame summer(DateTime(2026, 6, 22), DateTime(2026, 9, 23, 23, 59, 59));  // Verano 2026
  Frame autumn(DateTime(2026, 9, 24), DateTime(2026, 12, 31, 23, 59, 59)); // Otoño 2026 ampliado

  Program pumbProgramWinter(5, 0, NO_DAYS, &winter);
  Program pumbProgramSpring(5, 0, NO_DAYS, &spring);
  Program pumbProgramSummer(5, 0, NO_DAYS, &summer);
  Program pumbProgramAutumn(5, 0, NO_DAYS, &autumn);

  // Program e1ProgramWinter(5, 3, NO_DAYS, &winter);
  // Program e1ProgramSpring(5, 3, NO_DAYS, &spring);
  // Program e1ProgramSummer(5, 3, NO_DAYS, &summer);
  // Program e1ProgramAutumn(5, 3, NO_DAYS, &autumn);

  // Program e2ProgramWinter(5, 3, NO_DAYS, &winter);
  // Program e2ProgramSpring(5, 3, NO_DAYS, &spring);
  // Program e2ProgramSummer(5, 3, NO_DAYS, &summer);
  // Program e2ProgramAutumn(5, 3, NO_DAYS, &autumn);

  // Program e3ProgramWinter(5, 3, SATURDAY, &winter);
  // Program e3ProgramSpring(5, 3, MON_WED_SAT, &spring);
  // Program e3ProgramSummer(5, 3, MON_WED_SAT, &summer);
  // Program e3ProgramAutumn(5, 3, SATURDAY, &autumn);
  
//For test
  Program e1ProgramWinter(5, 3, DAILY, &winter);
  Program e1ProgramSpring(5, 3, DAILY, &spring);
  Program e1ProgramSummer(9, 3, DAILY, &summer);
  Program e1ProgramAutumn(5, 3, DAILY, &autumn);

  Program e2ProgramWinter(5, 3, DAILY, &winter);
  Program e2ProgramSpring(5, 3, DAILY, &spring);
  Program e2ProgramSummer(10, 3, DAILY, &summer);
  Program e2ProgramAutumn(5, 3, DAILY, &autumn);

  Program e3ProgramWinter(5, 3, DAILY, &winter);
  Program e3ProgramSpring(5, 3, DAILY, &spring);
  Program e3ProgramSummer(11, 3, DAILY, &summer);
  Program e3ProgramAutumn(5, 3, DAILY, &autumn);
  


  Button E1Button(9);
  Button E2Button(10);
  Button E3Button(11);
  Button ModeButton(8);

  FlowSensor flowSensor(2, 7.5); // Pin 2, kFactor 7.5 (ajustar según el sensor)
  
  Program* PumbProgram[4] = { 
      &pumbProgramWinter,
      &pumbProgramSpring,
      &pumbProgramSummer,
      &pumbProgramAutumn
  };

  Pumb pumb(0, PumbProgram, 4);

  Program* E1Programs[4] = { 
      &e1ProgramWinter,
      &e1ProgramSpring,
      &e1ProgramSummer,
      &e1ProgramAutumn
  };

  Program* E2Programs[4] = { 
      &e2ProgramWinter,
      &e2ProgramSpring,
      &e2ProgramSummer,
      &e2ProgramAutumn
  };

  Program* E3Programs[4] = { 
      &e3ProgramWinter,
      &e3ProgramSpring,
      &e3ProgramSummer,
      &e3ProgramAutumn
  };
  
  // --- ELECTROVÁLVULAS Y CONTROLADOR ---

  // 3. CORRECCIÓN: Creación de objetos en el stack (sin 'new')
  // El argumento para Electrovalve es la dirección del primer elemento del array: &E1Programs[0].
  
  Electrovalve E1(1, E1Programs, 5);
  Electrovalve E2(2, E2Programs, 6); // Usando E2Programs
  Electrovalve E3(3, E3Programs, 7); // Usando E3Programs


  // 3. Agruparlas en arrays (listas)
  Electrovalve* listaValvulas[] = {&E1, &E2, &E3};
  Button* listaBotones[] = {&E1Button, &E2Button, &E3Button};
  const uint8_t totalValvulas = sizeof(listaValvulas) / sizeof(listaValvulas[0]);

  // 4.  
  // Asumiendo que la clase se llama Controller, se pasa la referencia a los obConjetos.
  Controller rec(listaValvulas, listaBotones, totalValvulas, &pumb, &ModeButton, &soilSensor, &thSensor);

void setup() {
  Serial.begin(115200);
  Serial.println(F("Sistema de Riego Iniciado a 115200 baud"));
  Serial.println(F("Envíe 'd' para volcar los logs de la EEPROM"));
  E1Button.init();
  E2Button.init();
  E3Button.init();
  ModeButton.init();
  E1.init();
  E2.init();
  E3.init();
  pumb.init();
  flowSensor.init();
  soilSensor.init();
  display.init();
  clock.init();
  rec.init(); 
  
}

void loop() {
  rec.check(); 
  delay(100); // Reducido el delay para mejorar la respuesta de botones/serial
}
