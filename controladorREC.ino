#include <Wire.h>
#include <Arduino.h>
#include <RTClib.h> // Necesario para DateTime
#include "Controler.h"
#include "Frame.h"
#include "Electrovalve.h"
#include "Program.h"

// Definiciones de arrays de días fijos (necesario si Program espera un const uint8_t days[])
const uint8_t NO_DAYS[7] = {0, 0, 0, 0, 0, 0, 0};
const uint8_t SATURDAY[7] = {0, 0, 0, 0, 0, 0, 1};
const uint8_t MON_WED_SAT[7] = {0, 1, 0, 1, 0, 0, 1}; 

void setup() {
  Serial.begin(9600);
  Serial.println("Init Serial at 9600");

  // 1. CORRECCIÓN: Inicialización de objetos temporales DateTime
  // Se usa la sintaxis estándar de constructor C++: Tipo(param1, param2...)
  // y se elimina 'new' para objetos stack.

  // Usamos el constructor de enteros para evitar problemas con la cadena.
  Frame winter(DateTime(2025, 12, 22), DateTime(2026, 3, 20, 23, 59, 59));
  Frame spring(DateTime(2026, 3, 21), DateTime(2026, 6, 21, 23, 59, 59));
  Frame summer(DateTime(2026, 6, 22), DateTime(2026, 9, 23, 23, 59, 59));
  Frame autumn(DateTime(2026, 9, 24), DateTime(2026, 12, 21, 23, 59, 59));

  
  
  Program E1Programs[4] = { 
      Program(5, 10, NO_DAYS, &winter), 
      Program(5, 10, NO_DAYS, &spring), 
      Program(5, 10, NO_DAYS, &summer),
      Program(5, 10, NO_DAYS, &autumn)
  };

  Program E2Programs[4] = { 
      Program(5, 10, NO_DAYS, &winter), 
      Program(5, 10, NO_DAYS, &spring), 
      Program(5, 10, NO_DAYS, &summer),
      Program(5, 10, NO_DAYS, &autumn)
  };

  Program E3Programs[4] = { 
      Program(5, 10, SATURDAY, &winter), 
      Program(5, 10, SATURDAY, &spring), 
      Program(5, 10, MON_WED_SAT, &summer),
      Program(5, 10, SATURDAY, &autumn)
  };
  
  // --- ELECTROVÁLVULAS Y CONTROLADOR ---

  // 3. CORRECCIÓN: Creación de objetos en el stack (sin 'new')
  // El argumento para Electrovalve es la dirección del primer elemento del array: &E1Programs[0].
  
  Electrovalve E1(&E1Programs[0], 5);
  Electrovalve E2(&E2Programs[0], 6); // Usando E2Programs
  Electrovalve E3(&E3Programs[0], 7); // Usando E3Programs

  // 4. CORRECCIÓN: Nombre de clase 'Controller' debe coincidir con el include 'Controler.h'. 
  // Asumiendo que la clase se llama Controller, se pasa la referencia a los objetos.
  Controler rec(E1, E2, E3);
}

void loop() {
  delay(1000);
}
