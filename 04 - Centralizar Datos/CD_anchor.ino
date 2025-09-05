/* CENTRALIZAR DATOS EN UN ANCHOR */

/*Este es el código para el/los anchor(s).  Hay que configurar uno de ellos 
como el maestro, y el resto como esclavos. 
Esto lo hago cambiando la constante IS_MASTER de true a false según sea.
Además, debo variar también el ANCHOR_ADD del resto para que no coincidan*/

#include <SPI.h>
#include "DW1000Ranging.h"
#include "DW1000.h"

//DEBUG MODE
#define DEBUG 1


//Definiciones de Pines de la placa usada:
#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23
#define DW_CS 4

const uint8_t PIN_RST = 27; // reset pin
const uint8_t PIN_IRQ = 34; // irq pin
const uint8_t PIN_SS = 4;   // spi select pin

// Definiciones propias del Anchor:
#define DEVICE_ADDR "A1:00:5B:D5:A9:9A:E2:9C" 
uint16_t own_short_addr = DW1000Ranging.getCurrentShortAddress();
uint16_t Adelay = 16580;
#define IS_MASTER true
//#define IS_MASTER false

// Estructura para gestionar las medidas recibidas en el maestro.
struct Medida {
    uint16_t shortAddr_origin;
    uint16_t shortAddr_destiny;   
    float distance;       // Last measured distance (in meters)
    float rxPower;        // Last RX power measured with the destiny (in dBm)
     bool active;        // Checks if the destiny device is active. 
};

//Variables y constantes para registrar las medidas recibidas
#define MAX_DISPOSITIVOS 5
Medida medidas[MAX_DISPOSITIVOS];
int numDispositivos = 0;

//Variables y constantes para mostrar los resultados por el monitor serie:
unsigned long last_print = 0;   // Momento del último print
unsigned long current_time = 0; // Instante actual. Lo usaré para calcular las diferencias
const unsigned long refresh_time = 40000; //Hago un print de los datos centralizados cada 40s
const unsigned long refresh_time_debug = 1000; //Hago un print cada 1000 ms 

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
        //Master's callbacks: 
        // For when the slaves send a data report:
        DW1000Ranging.attachDataReport(DataReport);
              
        DW1000Ranging.startAsInitiator(DEVICE_ADDR,DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
        // This means: the anchor starts the communication. 
        // In the library, this is called: act as an Initiator 
    }
    else{

        //Callbacks for the slave anchors:
        
        //1: They must respond to a change request message (Sent by the master)
        DW1000Ranging.attachModeChangeRequest(ModeChangeRequest);

        //2: Must answer to a data request message (also sent by master)
        DW1000Ranging.attachDataRequested(DataRequest);
        
        //Finally, slaves are started as responders:
        DW1000Ranging.startAsResponder(DEVICE_ADDR,DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);  

    } 
}

int searchDevice(uint16_t own_sa,uint16_t dest_sa){
    
    for (int i=0 ; i < numDispositivos ; i++){

        if ((medidas[i].shortAddr.origin == own_sa)&&(medidas[i].shortAddr_destiny == dest_sa)) {
            return i; 
            // If found, returns the index
            
        }
    }
    return -1; // if not, returns -1
}

void logMeasure(uint16_t own_sa, uint16_t dest_sa, float dist, float rx_pwr){

    int index = searchDevice(own_sa,dest_sa);

    if (index != -1){ // This means: it was found.

        medidas[index].distance = dist; 
        medidas[index].rxPower = rx_pwr; 
        medidas[index].active = true;

    }
    else if (numDispositivos < MAX_DISPOSITIVOS){
        // If not found, i need to make a new entry to the struct.
        medidas[numDispositivos].shortAddr_origin = own_sa;
        medidas[numDispositivos].shortAddr_destiny = dest_sa;
        medidas[numDispositivos].distance = dist;
        medidas[numDispositivos].rxPower = rx_pwr;
        medidas[numDispositivos].active = true;
        numDispositivos ++; // And increase the device number in 1.
    }
    else{
        Serial.println("Lista de dispositivos llena");
    }
}


void DataReport(const byte* data){

    uint8_t index = SHORT_MAC_LEN + 1;

    uint16_t origin_short_addr = ((uint16_t)data[index] << 8) | data[index + 1];
    index += 2;

    uint8_t numMedidas = data[index++];

    for (int i = 0; i < numMedidas; i++) {

        uint16_t destiny_short_addr = ((uint16_t)data[index] << 8) | data[index + 1];
        index += 2;

        float distance, rxPower;
        memcpy(&distance, data + index, 4); index += 4;
        memcpy(&rxPower,   data + index, 4); index += 4;

        
        logMeasure(origin_short_addr, destiny_short_addr, distance, rxPower);
    }
}

void DataRequest(void){

    // Called when the master sends the slave a data request.
    // The slave answers by sending the data report:

    DW1000Ranging.transmitDataReport(medidas,numDispositivos);


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

void showData(){

    Serial.println("--------- NUEVA MEDIDA ---------");
    
    for (int i = 0; i < numDispositivos ; i++){ 
        if(medidas[i].active == true){
            
            Serial.print(" Dispositivos: ");
            Serial.print(medidas[i].shortAddr_origin,HEX);
            Serial.print(" -> ");
            Serial.print(medidas[i].shortAddr_destiny,HEX);
            Serial.print("\t Distancia: ");
            Serial.print(medidas[i].distance);
            Serial.print(" m \t RX power: ");
            Serial.print(medidas[i].rxPower);
            Serial.println(" dBm");
        }
    }
    Serial.println("--------------------------------");
}

void showData_debug(){

    Serial.println("--------- NUEVA MEDIDA ---------");
    
    for (int i = 0; i < numDispositivos ; i++){ 
        if(medidas[i].active == true){
            
            Serial.print(" Dispositivos: ");
            Serial.print(medidas[i].shortAddr_origin,HEX);
            Serial.print(" -> ");
            Serial.print(medidas[i].shortAddr_destiny,HEX);
            Serial.print("\t Distancia: ");
            Serial.print(medidas[i].distance);
            Serial.print(" m \t RX power: ");
            Serial.print(medidas[i].rxPower);
            Serial.println(" dBm");
        }
    }
    Serial.println("--------------------------------");
    
}

void newRange(){

    uint16_t dest_sa = DW1000Ranging.getDistantDevice()->getShortAddress();
    float dist = DW1000Ranging.getDistantDevice()->getRange();
    float rx_pwr = DW1000Ranging.getDistantDevice()->getRXPower();

    logMeasure(own_short_addr,dest_sa, dist, rx_pwr);

}

void newDevice(DW1000Device *device){

    Serial.print("Nuevo dispositivo: ");
    Serial.println(device->getShortAddress(), HEX);
}

void inactiveDevice(DW1000Device *device){

    uint16_t dest_sa = device->getShortAddress();
    int index = searchDevice(sa);

    Serial.print("Conexión perdida con el dispositivo: ");
    Serial.println(dest_sa, HEX);

    //Al estar inactivo, pongo en false esa propiedad:
    medidas[index].active = false;
}

void loop(){

    DW1000Ranging.loop();
    current_time = millis();
    if(DEBUG){

        if(current_time - last_print >= refresh_time_debug){

            showData_debug();
            last_print = millis();
        }
    }
    else{
        if(current_time - last_print >= refresh_time){

            showData();
            last_print = millis();
        }
        else if(IS_MASTER && current_time - last_switch >= switch_time){

            last_switch = millis();
            delay(100);
            DW1000Ranging.transmitModeSwitch(currentModeisInitiator);
            //Only 1 parameter: a boolean to indicate which mode I want to switch to:
            // true = toInitiator
        
            // 2nd parameter is the target device. 
            // If null --> Broadcast (to all devices listening)
            // If != 0, message is sent to the specified device. 
            delay(100);
            currentModeisInitiator = !currentModeisInitiator;
        }
    }
}