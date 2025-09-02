/* CENTRALIZAR DATOS EN UN ANCHOR */

// Este anchor será el que recopile todas las distancias medidas. 

#include <SPI.h>
#include "DW1000Ranging.h"
#include "DW1000.h"

//Definiciones de Pines de la placa usada:
#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23
#define DW_CS 4

const uint8_t PIN_RST = 27; // reset pin
const uint8_t PIN_IRQ = 34; // irq pin
const uint8_t PIN_SS = 4;   // spi select pin

// Definiciones propias del Anchor:
#define ANCHOR_ADD "A1:17:5B:D5:A9:9A:E2:9C" 
uint16_t Adelay = 16580;
#define IS_MASTER true;



// Estructura para gestionar las medidas recibidas en el maestro.
struct Medida {
    uint16_t shortAddr;   // ID del dispositivo desde el que se recibe
    float distancia;      // Última distancia medida (m)
    float rxPower;        // Última potencia recibida (dBm)
};

#define CANT_DISPOSITIVOS 5
Medida medidas[CANT_DISPOSITIVOS];
int numDispositivos = 0;


void startAsMasterAnchor(){
    //Esto es, que el anchor inicie la comunicación. 
    // En la librería, eso lo llaman: actuar como un tag: 
    startAsTag(ANCHOR_ADD,DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
}

void startAsSlaveAnchor(){
    //En la inicialización, no quiero que esté haciendo poll. Comienza como anchor "normal". Responderá al anchor maestro para medir la posición entre ambos.
    startAsAnchor(ANCHOR_ADD,DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
}

void setup(){

    Serial.begin(115200);
    delay(1000); // Le doy 1 segundo para el monitor serie

    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI); // Inicializo el bus SPI
    DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); // Start del DW1000

    DW1000.setAntennaDelay(Adelay);

    //"Habilito" los callbacks: 
    DW1000Ranging.attachNewRange(newRange);
    DW1000Ranging.attachNewDevice(newDevice);
    DW1000Ranging.attachInactiveDevice(inactiveDevice);   
    
    if (IS_MASTER){
        startAsMasterAnchor();
    }
    else startAsSlaveAnchor();  
}

void registrarMedida(uint16_t shortAddress, float dist, float rx){

    int i = 0;

    for i = 0
        if medidas[i].shortAddr == shortAddress{

            medidas[i].
        }

}

void newRange(){

    uint16_t sa = DW1000Ranging.getDistantDevice()->getShortAddress();
    float distancia = DW1000Ranging.getDistantDevice()->getRange();
    float rx_pwr = DW1000Ranging.getDistantDevice()->getRXPower();

    registrarMedida(sa, distancia, rx_pwr);

}

void newDevice(DW1000Device *device){

    // La librería DW1000 lanza este callback cuando detecta una comunicación desde un device con shortAdress distinta a las que ha visto hasta entonces.
    Serial.print("Nuevo dispositivo: ");
    Serial.println(device->getShortAddress(), HEX);
}

void inactiveDevice(DW1000Device *device){

    //Dentro de DWdevice.h, se define el inactivity_time como 1s.
    // Si pasa ese tiempo sin señal de un dispositivo que ya había registrado antes, lo considero inactivo. 
    Serial.print("Conexión perdida con el dispositivo: ");
    Serial.println(device->getShortAddress(), HEX);
}






