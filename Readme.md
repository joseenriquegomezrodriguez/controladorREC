# Controlador de Rec

Controlador de rec per Arduino amb la programació de rec carregada en el codi.

Permet aplicar factor de correcció del temps de rec en funció de la climatologia (humitat ambiental, temperatura i humitat del sol)

Controla la bomba d'aigua i un de 1 a N electrovalvules.

La programació del rec va lligada a les electrovalvules.

Els moment de reg es registren dins de l'EEPROM del rellotge.

També registre possibles problemes:

* Fuites d'aigua: consum d'aigua sense que estigui funcionant les electrovalvules o la bomba.
* Funcionament en sec: No hi ha fluxe d'aigua i la bomba esta funcionant.

## Elements necessaris

(En funció dels models s'ha d'adaptar el software)

* Arduino.
* Modul RTC (rellotge)
* Display (20x4)
* Sensor de Temperatura i Humitat ambiental.
* Sensor de humitat del terra.
* Caudalimetre.
* Buttons de control / keypad

## Display

```text
 --------------------
|CONTROL DE REG E:xxx|
|T:xx M:AUTO E1 E2 E3|
|HT:xxx% VA:xx.xL/min|
|HA:xxx% D:xx H:00:00|
 --------------------
```

* E: Estació (PRI,EST,TAR,HIV)
* T: Temperatura ºC
* HT: Humitat Terra %
* HA: Humitat ambient %
* D: Dia de la setmana ("DG","DL","DT","DC","DJ","DV","DS)
* H: Hora actual HH:MM
* VA: Volúm d'aigua en L/min
* E1 / M1 / X1 : Electrovalvula 1 en estat Auto/Manual/Stop

## Ajustaments mitjançant el port serie

```text
--------------------CONTROLADOR DE RIEGO REC ------------------------
  Comandos serie disponibles:
  S<valor> : Calibrar punto SECO (0%) del sensor de suelo
  H<valor> : Calibrar punto HUMEDO (100%) del sensor de suelo
  T<UnixTime> : Ajustar Fecha/Hora (ej: T1783342270)
  D        : Descargar historial de logs desde la EEPROM
  R        : Borrar (Reset) todos los logs de la memoria
  L        : Mostrar este menu de ayuda
----------------------------------------------------------------------
```

Botons  (MODE)    (E1)  (E2)  (E3)

Si la pantalla s'apaga qualsevol boto l'encen.

Si la pantalla es encesa:
(MODE): Al presionar el boto canvia d'estat:

* AUTO El controlador funciona segons la programació del les electovalvules.
* MANU El controlador encen la bonba de forma manual.
* STOP El controlador s'atura. No funcionara cap programacion ni la bonba.

(Ex):Al presionar el boto canvia d'estat:

* Ex La electrovalvula 1 funciona amb la programació.
* Mx La electrovalvula 1 funciona manualment encenent la valvula i la bomba.
* Xx La electrovalvula 1 s'atura.

## Estats de l'aplicació

(Generat per IA)

![Estats del Controlador](/images/EstadoController.svg)
![Estats de les electrovalvules](/images/EstadosElectorvalve.svg)

## Diagrama de clases

```bash
$/>source ~/pythonenv/bin/activate
(pythonenv) $/>hpp2plantuml -i lib/Controller/Controller.h -i lib/Button/Button.h -i lib/Clock/Clock.h -i lib/Display/Display.h -i lib/CustomKeypad/CustomKeypad.h -i lib/Electrovalve/Electrovalve.h -i lib/FlowSensor/FlowSensor.h -i lib/Frame/Frame.h -i lib/Program/Program.h -i lib/Pumb/Pumb.h -i lib/Rele/Rele.h -i lib/SoilSensor/SoilSensor.h -i lib/THSensor/THSensor.h -o uml/diagrama.puml
```

![Diagrama de clases](/images/Diagrama.svg)
