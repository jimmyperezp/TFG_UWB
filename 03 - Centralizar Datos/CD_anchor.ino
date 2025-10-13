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

//Board's pins definitions:
#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23
#define DW_CS 4

const uint8_t PIN_RST = 27; // reset pin
const uint8_t PIN_IRQ = 34; // irq pin
const uint8_t PIN_SS = 4;   // spi select pin

// Definiciones del dispositivo
// Nomenclatura: A -> Anchors, B -> Tags
#define DEVICE_ADDR "A1:00:5B:D5:A9:9A:E2:9C"

uint16_t own_short_addr = 0; //La calculo después, una vez se ha iniciado el dispositivo.
uint16_t Adelay = 16580;
#define IS_MASTER true
//#define IS_MASTER false

//Variables y constantes para registrar las measurements recibidas
#define MAX_DISPOSITIVOS 5
Medida measurements[MAX_DISPOSITIVOS];

int numDispositivos = 0;

// Para controlar los tiempos y flujo de los datos
unsigned long current_time = 0; 
unsigned long last_switch = 0;
unsigned long last_print = 0;   
unsigned long last_report = 0;

const unsigned long refresh_time = 1000;  // DEBUG: Para mostrar el cambio en modo funcionamiento
const unsigned long switch_time = 10000; // Paso los esclavos de iniciador a respondedor cada 10 secs.
const unsigned long report_time = 40000; // Pido un data report cada 40 segundos. 

// Variable para controlar el modo actual del esclavo.
static bool currentModeisInitiator = false;

byte mode[] = DW1000.MODE_1;

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

        // Callbacks del maestro: 
        // Cuando el esclavo manda el data report:
        DW1000Ranging.attachDataReport(DataReport);
              
        DW1000Ranging.startAsInitiator(DEVICE_ADDR,mode, false);
        // Significa que el anchor se encarga de empezar la comunicación (hace polling)
       
    }
    else{

        //Callbacks de los anchors esclavos
        
        //1: Cuando se les pide cambiar de modo
        DW1000Ranging.attachModeChangeRequest(ModeChangeRequest);

        //2: Cuando se les pide un data report (reciben un data request)
        DW1000Ranging.attachDataRequested(DataRequest);
        
        // Comienzan respondiendo a los polls del maestro (comienzan como respondedores)
        DW1000Ranging.startAsResponder(DEVICE_ADDR,mode, false);  

    } 
    own_short_addr = getOwnShortAddress();
    // Tras haber sido iniciado, guardo su short address en un uint16_t.
}

int searchDevice(uint16_t own_sa,uint16_t dest_sa){
    
    for (int i=0 ; i < numDispositivos ; i++){

        if ((measurements[i].shortAddr_origin == own_sa)&&(measurements[i].shortAddr_destiny == dest_sa)) {
            return i; 
            // Si hay coincidencia, devuelve el índice
        }
    }
    return -1; // Si no, devuelve -1.
}

void logMeasure(uint16_t own_sa, uint16_t dest_sa, float dist, float rx_pwr){

    // Primero, busco si esa comunicación ya estaba registrada:
    int index = searchDevice(own_sa,dest_sa); 

    if (index != -1){ // Si sí que lo ha encontrado:

        //solo actualizo los valores de distancia y potencia.
        measurements[index].distance = dist; 
        measurements[index].rxPower = rx_pwr; 
        measurements[index].active = true;

    }
    else if (numDispositivos < MAX_DISPOSITIVOS){
        // Si no lo ha encontrado, guardo todo
        measurements[numDispositivos].shortAddr_origin = own_sa;
        measurements[numDispositivos].shortAddr_destiny = dest_sa;
        measurements[numDispositivos].distance = dist;
        measurements[numDispositivos].rxPower = rx_pwr;
        measurements[numDispositivos].active = true;
        numDispositivos ++; // And increase the device number in 1.
    }
    else{
        Serial.println("Lista de dispositivos llena");
    }
}


void DataReport(byte* data){

    uint8_t index = SHORT_MAC_LEN + 1;

    uint16_t origin_short_addr = ((uint16_t)data[index] << 8) | data[index + 1];
    index += 2;

    uint8_t numMeasures = data[index++];

    //Primero, comprueba la longitud de los datos recibidos: 

    if(numMeasures*10>LEN_DATA-SHORT_MAC_LEN-4){

        // Cada medida ocupa 10 bytes
        // El encabezado ocupa SHORT_MAC_LEN + 2 bytes del shortAddress + 1 byte del messageType + 1 byte del valor del numero de measurements enviado
        
        Serial.println("The Data received is too long");
        return;
    }

    //Si está ok, continúo extrayendo los datos que me interesan:

    for (int i = 0; i < numMeasures; i++) {

        uint16_t destiny_short_addr = ((uint16_t)data[index] << 8) | data[index + 1];
        index += 2;

        float distance, rxPower;
        memcpy(&distance, data + index, 4); index += 4;
        memcpy(&rxPower,   data + index, 4); index += 4;

        
        logMeasure(origin_short_addr, destiny_short_addr, distance, rxPower);
    }

    showData();
}


void DataRequest(byte* short_addr_requester){

    // El esclavo entra aquí cuando el maestro le ha enviado un data_request.
    // Le debe responder enviándole los datos:

    DW1000Device* requester = DW1000Ranging.searchDistantDevice(short_addr_requester);
    //1º guardo el dispositivo que lo ha pedido, para enviárselo solo a el

    if(!requester){
        // Si no lo encuentra, no lo envía a nadie.
        return;
    }

    uint16_t numMeasures = amountDevices;
    DW1000Ranging.transmitDataReport((Measurement*)measurements,numMeasures,requester);

}

void ModeChangeRequest(bool toInitiator){

    //The slave enters here when it receives a request to change its mode. 
    
    if(toInitiator == true){

        DW1000.idle();
        delay(100);
        DW1000Ranging.startAsInitiator(DEVICE_ADDR,mode, false);
        delay(100);
    }
    else{

        DW1000.idle();
        delay(100);
        DW1000Ranging.startAsResponder(DEVICE_ADDR,mode, false);
        delay(100);
        
    }
}

void showData(){

    Serial.println("--------- NUEVA MEDIDA ---------");
    
    for (int i = 0; i < numDispositivos ; i++){ 
        if(measurements[i].active == true){
            
            Serial.print(" Dispositivos: ");
            Serial.print(measurements[i].short_addr_origin,HEX);
            Serial.print(" -> ");
            Serial.print(measurements[i].short_addr_dest,HEX);
            Serial.print("\t Distancia: ");
            Serial.print(measurements[i].distance);
            Serial.print(" m \t RX power: ");
            Serial.print(measurements[i].rxPower);
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

    if (DEBUG){

        int index = searchDevice(own_short_addr,sa);
        if (index != 0){
             measurements[index].active = false;
        }

    }
}

void loop(){

    DW1000Ranging.loop();
    current_time = millis();
    if(DEBUG){

        if(current_time - last_print >= refresh_time){

            showData();
            last_print = millis();
        }
        else if(IS_MASTER && current_time - last_switch >= switch_time){

            last_switch = millis();
            delay(100);
            DW1000Ranging.transmitModeSwitch(currentModeisInitiator);

            //Solo le mando un parámetro: el booleano del modo al que quiero cambiar:
            // true = toInitiator
        
            // El 2º parámetro es el dispositivo al que se lo pido.  
            // Si lo dejo en blanco --> Broadcast (Lo envía a todos los dispositivos que estén escuchando)
             
            delay(100);
            currentModeisInitiator = !currentModeisInitiator;
        }
    }
    
    else{
    
        if (IS_MASTER){

            if(current_time - last_switch >= switch_time){

                last_switch = millis();
                delay(100);
                DW1000Ranging.transmitModeSwitch(currentModeisInitiator);

                delay(100);
                currentModeisInitiator = !currentModeisInitiator;
            }

            
            else if (current_time - last_report >= report_time){

                DW1000Ranging.transmitDataRequest();
                last_report = millis();
            }
        }
        
    }
}