/* CENTRALIZAR DATOS EN UN ANCHOR */

/*Este es el código para el/los anchor(s).  Hay que configurar uno de ellos 
como el maestro, y el resto como esclavos. 
Esto lo hago cambiando la constante IS_MASTER de true a false según sea.
Además, debo variar también el DEVICE_ADDR del resto para que no coincidan*/

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
// NOMENCLATURA: A para Anchors, B para Tags
#define DEVICE_ADDR "A1:00:5B:D5:A9:9A:E2:9C" 
uint16_t Adelay = 16580;
#define IS_MASTER true
//#define IS_MASTER false

// Estructura para gestionar las medidas recibidas en el maestro.
struct Medida {
    uint16_t shortAddr;   // ID del dispositivo desde el que se recibe
    float distancia;      // Última distancia medida (m)
    float rxPower;        // Última potencia recibida (dBm)
    bool activo;        //Para saber si está activo o no, y evitar mostrarlo por pantalla.
};

//Variables y constantes para registrar las medidas recibidas
#define MAX_DISPOSITIVOS 5
Medida medidas[MAX_DISPOSITIVOS];
int numDispositivos = 0;

//Variables y constantes para mostrar los resultados por el monitor serie:
unsigned long last_print = 0;   // Momento del último print
unsigned long current_time = 0; // Instante actual. Lo usaré para calcular las diferencias
const unsigned long refresh_time = 1000; //Hago un print cada 1000 ms 

// Variables y constantes para hacer cambio en el rol de los anchors esclavos: 
//1: Time management: 
unsigned long last_switch = 0;
const unsigned long switch_time = 10000;

//2: Current mode management: 
static bool currentModeisInitiator = false;

//CÓDIGO:

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
        //Esto es, que el anchor inicie la comunicación. 
        // En la librería, eso lo llaman: actuar como un iniciador: 
        DW1000Ranging.startAsInitiator(DEVICE_ADDR,DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
    }
    else{
        
        DW1000Ranging.attachModeChangeRequest(ModeChangeRequest);
        //En la inicialización, no quiero que esté haciendo poll. Comienza como anchor "normal". Responderá al anchor maestro para medir la posición entre ambos.
        DW1000Ranging.startAsResponder(DEVICE_ADDR,DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
    } 
}

int buscarDispositivo(uint16_t sa){
    
    for (int i=0 ; i < numDispositivos ; i++){

        if (medidas[i].shortAddr == sa) {
            return i; 
            // Si sí que hay coincidencia, devuelve el índice
        }
    }
    return -1; //si no se ha encontrado coincidencia
}

void registrarMedida(uint16_t sa, float dist, float rx_pwr){

    int index = buscarDispositivo(sa);

    if (index != -1){ //Esto es: si sí que hay coincidencia

        medidas[index].distancia = dist; 
        medidas[index].rxPower = rx_pwr; 
        medidas[index].activo = true;

    }
    else if (numDispositivos < MAX_DISPOSITIVOS){
        //Si no hay coincidencia, registro uno nuevo:
        medidas[numDispositivos].shortAddr = sa;
        medidas[numDispositivos].distancia = dist;
        medidas[numDispositivos].rxPower = rx_pwr;
        medidas[numDispositivos].activo = true;
        numDispositivos ++; //aumento en 1 la lista de dispositivos registrados.
    }
    else{
        Serial.println("Lista de dispositivos llena");
    }
}

void ModeChangeRequest(bool toInitiator){

    if(toInitiator == true){

        DW1000.idle();
        delay(100);
        DW1000Ranging.startAsInitiator(DEVICE_ADDR,DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
        delay(100);
    }
    else{

        DW1000.idle();
        delay(100);
        DW1000Ranging.startAsResponder(DEVICE_ADDR,DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
        delay(100);
    }
} 

void MostrarDatos(){

    byte* addr = DW1000Ranging.getCurrentShortAddress();

    Serial.println("--------- NUEVA MEDIDA ---------");
    
    for (int i = 0; i < numDispositivos ; i++){ 
        if(medidas[i].activo == true){
            Serial.print(" Dispositivos: ");
            Serial.print(addr[0],HEX);
            Serial.print(" -> ");
            Serial.print(medidas[i].shortAddr,HEX);
            Serial.print("\t Distancia: ");
            Serial.print(medidas[i].distancia);
            Serial.print(" m \t RX power: ");
            Serial.print(medidas[i].rxPower);
            Serial.println(" dBm");
        }
    }
    Serial.println("--------------------------------");
}


void newRange(){

    uint16_t sa = DW1000Ranging.getDistantDevice()->getShortAddress();
    float dist = DW1000Ranging.getDistantDevice()->getRange();
    float rx_pwr = DW1000Ranging.getDistantDevice()->getRXPower();

    registrarMedida(sa, dist, rx_pwr);

}

void newDevice(DW1000Device *device){

    Serial.print("Nuevo dispositivo: ");
    Serial.println(device->getShortAddress(), HEX);
}

void inactiveDevice(DW1000Device *device){

    uint16_t sa = device->getShortAddress();
    int index = buscarDispositivo(sa);

    Serial.print("Conexión perdida con el dispositivo: ");
    Serial.println(sa, HEX);

    //Al estar inactivo, pongo en false esa propiedad:
    medidas[index].activo = false;
}

void loop(){

    DW1000Ranging.loop();
    current_time = millis();

    if(current_time - last_print >= refresh_time){
        MostrarDatos();
        last_print = millis();
    }
    else if(IS_MASTER && current_time - last_switch >= switch_time){

        last_switch = millis();
        delay(100);
        DW1000Ranging.transmitModeSwitch(currentModeisInitiator);
        // Solo le paso 1 parámetro -> el modo que quiero: true = pasar a iniciador
        // El segundo parámetro es null -> Hace broadcast: se lo pide a todos los slave_anchors
        delay(100);
        currentModeisInitiator = !currentModeisInitiator;
    }

}