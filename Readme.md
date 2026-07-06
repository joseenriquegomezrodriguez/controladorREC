# Controlador de Rec

Controlador de rec per Arduino amb la programació de rec carregada en el codi.

Permet aplicar factor de correcció del temps de rec en funció de la climatologia (humitat ambiental, temperatura i humitat del sol)

Controla la bomba d'aigua i un de 1 a N electrovalvules.

La programació del rec va lligada a les electrovalvules.

Els moment de reg es registren dins de l'EEPROM del rellotge.

També registre possibles problemes:
Fuites d'aigua: consum d'aigua sense que estigui funcionant les electrovalvules o la bomba.
Funcionament en sec: No hi ha fluxe d'aigua i la bomba esta funcionant.

Elements necessaris:
* Arduino.
* Modul RTC (rellotge)
* Display (20x4)
* Sensor de Temperatura i Humitat ambiental.
* Sensor de humitat del terra.
* Caudalimetre.
* Buttons de control / keypad

## Display
<pre>
 --------------------
|CONTROL DE REG E:xxx|
|T:xx M:AUTO E1 E2 E3|
|HT:xxx% VA:xx.xL/min|
|HA:xxx% D:xx H:00:00|
 --------------------
</pre>

- E: Estació (PRI,EST,TAR,HIV)
- T: Temperatura ºC
- HT: Humitat Terra %
- HA: Humitat ambient %
- D: Dia de la setmana ("DG","DL","DT","DC","DJ","DV","DS)
- H: Hora actual HH:MM
- VA: Volúm d'aigua en L/min
- E1 / M1 / X1 : Electrovalvula 1 en estat Auto/Manual/Stop



## Ajustaments mitjançant el port serie:
<pre>
--------------------CONTROLADOR DE RIEGO REC ------------------------
  Comandos serie disponibles:
  S<valor> : Calibrar punto SECO (0%) del sensor de suelo
  H<valor> : Calibrar punto HUMEDO (100%) del sensor de suelo
  T<UnixTime> : Ajustar Fecha/Hora (ej: T1783342270)
  D        : Descargar historial de logs desde la EEPROM
  R        : Borrar (Reset) todos los logs de la memoria
  L        : Mostrar este menu de ayuda
----------------------------------------------------------------------
</pre>

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

<img title="Estats de l'aplicació" alt="Estats" src="/images/Estados.svg">


## Diagrama de clases


<img title="Diagrama de clases" alt="Diagrama" src="/images/Diagrama.svg">
