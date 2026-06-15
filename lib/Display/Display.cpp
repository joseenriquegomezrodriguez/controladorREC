//Display.cpp
#include <Arduino.h>
#include "Display.h"

Display::Display(): lcd(0x27, 20, 4){
    // No llamar a init() aquí. Dejar que setup() lo haga.
    // Esto asegura que Wire.begin() se haya ejecutado primero.
};
void Display::setON(){
    lcd.backlight();
    backLight = true;
};
void Display::setOFF(){
    lcd.noBacklight();
    backLight = false;
};
void Display::init(){
    lcd.init();                      // Inicializa el hardware del LCD
    lcd.clear();                     // Limpia cualquier residuo en la memoria del LCD
    Display::setON();
    lcd.setCursor(0,0);
    lcd.print(F("CONTROL DE REG E:xxx"));
    lcd.setCursor(0,1);
    lcd.print(F("T:xx M:AUTO E1 E2 E3"));
    lcd.setCursor(0,2);
    lcd.print(F("HT:xxxx VA:xx.xL/min"));
    lcd.setCursor(0,3);
    lcd.print(F("HA:xx% D:xx H:xx:xx"));
    // lcd.print(F("E:xxx T:xx M:AUTO"));
    // lcd.setCursor(0,1);
    // lcd.print(F("E1 E2 E3 E4 E5 E6 E7"));
    // lcd.setCursor(0,2);
    // lcd.print(F("HT:xxxx VA:xx.xL/min"));
    // lcd.setCursor(0,3);
    // lcd.print(F("HA:xx% D:xx H:xx:xx"));

};
bool Display::getBackLight(){
    return backLight;
};
void Display::print(uint8_t X, uint8_t Y, const char* txt ){
    lcd.setCursor(X,Y);
    lcd.print(txt);
};

void Display::printValveStatus(uint8_t index, char* text) {
    // Calculamos la columna inicial
    uint8_t col = pos_EV[0] + (index * 3);
    uint8_t row = pos_EV[1];

    // Si la columna excede el límite del LCD (20 caracteres), 
    // debemos manejar el salto de línea manualmente para que no sea errático
    if (col >= 20) {
        // Ejemplo: Si tienes más de 2-3 válvulas, podrías querer 
        // mover las siguientes a la línea 2 (índice 2)
        // Por ahora, esto evita que el hardware decida por ti:
        col = (index - 3) * 3; // Empezar desde la izquierda para la válvula 4
        row = 2;               // Mover a la fila 2 (tercera línea)
        // Nota: Esto requeriría que tu layout en init() deje espacio en la línea 2
    }

    print(col, row, text);
}

void Display::printHour(const char* txt){
    print(pos_H[0], pos_H[1], txt);
};
void Display::printDay(const char* txt){
    print(pos_D[0], pos_D[1], txt);
};
void Display::printStation(const char* txt){
    print(pos_E[0], pos_E[1], txt);
};
void Display::printMode(const char* txt){
    print(pos_M[0], pos_M[1], txt);
};

void Display::printFlow(float flow) {
    char buffer[6];
    dtostrf(flow, 4, 1, buffer); // Formato "xx.x" (4 caracteres)
    print(pos_VA[0], pos_VA[1], buffer);
};
void Display::printSoilMoisture(int soilMoisture) {
    char buffer[6]; // Espacio suficiente para "100%" + el terminador nulo
    char clearBuffer[] = "    "; // El compilador asigna automáticamente el tamaño necesario (5 bytes)
    snprintf(buffer, sizeof(buffer), "%d%%", soilMoisture);
    print(pos_HT[0], pos_HT[1], clearBuffer); // Limpia el espacio antes de imprimir el nuevo valor
    print(pos_HT[0], pos_HT[1], buffer);    // Imprime en la esquina superior izquierda de
};
void Display::printHumidity(int humidity) {
    char buffer[5];
    char clearBuffer[] = "    "; // El compilador asigna automáticamente el tamaño necesario (5 bytes)
    snprintf(buffer, sizeof(buffer), "%d%%", humidity);
    print(pos_HA[0], pos_HA[1], clearBuffer); // Limpia el espacio antes de imprimir el nuevo valor
    print(pos_HA[0], pos_HA[1], buffer);
};
void Display::printTemperature(int temperature) {
    char buffer[5];
    char clearBuffer[] = "  "; // El compilador asigna automáticamente el tamaño necesario (5 bytes)
    snprintf(buffer, sizeof(buffer), "%d", temperature);
    print(pos_T[0], pos_T[1], clearBuffer); // Limpia el espacio antes de imprimir el nuevo valor
    print(pos_T[0], pos_T[1], buffer);
};