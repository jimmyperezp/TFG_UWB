// AUTOCALIBRADO DEL ANTENNA DELAY (PARA ANCHOR)

/*PROCEDIMIENTO: 

  1) Situar Anchor y Tag a distancia conocida
  2) Introducir esa distancia en la variable this_anchor_target_distance (línea 30)
  3) Subir el programa al anchor
  4) Guardar la medida del antenna delay que muestra por el monitor serie
  5) Repetir este proceso si se observan medidas erróneas, hasta afinar el resultado.
*/

// El tag tiene que estar configurado con el antenna delay default (library default = 16384)

#include <SPI.h>
#include "DW1000Ranging.h"
#include "DW1000.h"

// ESP32_UWB pin definitions
#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23
#define DW_CS 4

// connection pins
const uint8_t PIN_RST = 27; // reset pin
const uint8_t PIN_IRQ = 34; // irq pin
const uint8_t PIN_SS = 4;   // spi select pin

#define DEVICE_ADDR "A1:00:5B:D5:A9:9A:E2:9C"
float this_anchor_target_distance = 1; //distancia anchor-tag en metros.

uint16_t this_anchor_Adelay = 16600; //starting value
uint16_t Adelay_delta = 100; //initial binary search step size

void setup(){

  Serial.begin(115200);
  while (!Serial);

  //init the configuration
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); //Reset, CS, IRQ pin

  Serial.print("Starting Adelay "); Serial.println(this_anchor_Adelay);
  Serial.print("Measured distance "); Serial.println(this_anchor_target_distance);
  
  DW1000.setAntennaDelay(this_anchor_Adelay);

  DW1000Ranging.attachNewRange(newRange);
  DW1000Ranging.attachNewDevice(newDevice);
  DW1000Ranging.attachInactiveDevice(inactiveDevice);
  //Enable the filter to smooth the distance
  //DW1000Ranging.useRangeFilter(true);

  //start the module as anchor, don't assign random short address
  DW1000Ranging.DEVICE_ADDR(this_anchor_addr, DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
}

void loop(){

  DW1000Ranging.loop();
}

void newRange(){

  static float last_delta = 0.0;
  Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress(), DEC);

    float dist = DW1000Ranging.getDistantDevice()->getRange();

  Serial.print(",");
  Serial.print(dist); 
  if (Adelay_delta < 3) {
    Serial.print(", final Adelay ");
    Serial.println(this_anchor_Adelay);
//    Serial.print("Check: stored Adelay = ");
//    Serial.println(DW1000.getAntennaDelay());
    while(1);  //done calibrating
  }

  float this_delta = dist - this_anchor_target_distance;  //error in measured distance

  if ( this_delta * last_delta < 0.0) Adelay_delta = Adelay_delta / 2; //sign changed, reduce step size
    last_delta = this_delta;
  
  if (this_delta > 0.0 ) this_anchor_Adelay += Adelay_delta; //new trial Adelay
  else this_anchor_Adelay -= Adelay_delta;
  
  Serial.print(", Adelay = ");
  Serial.println (this_anchor_Adelay);
//  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); //Reset, CS, IRQ pin
  DW1000.setAntennaDelay(this_anchor_Adelay);
}

void newDevice(DW1000Device *device)
{
  Serial.print("Device added: ");
  Serial.println(device->getShortAddress(), HEX);
}

void inactiveDevice(DW1000Device *device)
{
  Serial.print("delete inactive device: ");
  Serial.println(device->getShortAddress(), HEX);
}