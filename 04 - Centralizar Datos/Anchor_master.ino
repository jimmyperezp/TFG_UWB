/* CENTRALIZAR DATOS EN UN ANCHOR */

/*Este es el código para el/los anchor(s).  Hay que configurar uno de ellos 
como el maestro, y el resto como esclavos. 
Esto lo hago cambiando la constante IS_MASTER de true a false según sea.
Además, debo variar también el ANCHOR_ADD del resto para que no coincidan*/

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
#define DEVICE_ADDR "A1:00:5B:D5:A9:9A:E2:9C" 
uint16_t Adelay = 16580;
#define IS_MASTER true
//#define IS_MASTER false

// Estructura para gestionar las medidas recibidas en el maestro.
struct Medida {
    uint16_t shortAddr;   // ID del dispositivo desde el que se recibe
    float distancia;      // Última distancia medida (m)
    float rxPower;        // Última potencia recibida (dBm)
};

//Variables y constantes para registrar las medidas recibidas
#define MAX_DISPOSITIVOS 5
Medida medidas[MAX_DISPOSITIVOS];
int numDispositivos = 0;

//Variables y constantes para mostrar los resultados por el monitor serie:
unsigned long last_print = 0;   // Momento del último print
unsigned long current_time = 0; // Instante actual. Lo usaré para calcular las diferencias
const unsigned long refresh_time = 1000; //Hago un print cada 1000 ms 



//CÓDIGO:

void startAsMasterAnchor(){
    //Esto es, que el anchor inicie la comunicación. 
    // En la librería, eso lo llaman: actuar como un tag: 
    DW1000Ranging.startAsTag(DEVICE_ADDR,DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
}

void startAsSlaveAnchor(){
    //En la inicialización, no quiero que esté haciendo poll. Comienza como anchor "normal". Responderá al anchor maestro para medir la posición entre ambos.
    DW1000Ranging.startAsAnchor(DEVICE_ADDR,DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
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
    DW1000Ranging.attachModeChangeRequestHandler(ModeChangeRequest);

    
    if (IS_MASTER){
        startAsMasterAnchor();
    }
    else startAsSlaveAnchor();  
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

    }
    else if (numDispositivos < MAX_DISPOSITIVOS){
        //Si no hay coincidencia, registro uno nuevo:
        medidas[numDispositivos].shortAddr = sa;
        medidas[numDispositivos].distancia = dist;
        medidas[numDispositivos].rxPower = rx_pwr;
        numDispositivos ++; //aumento en 1 la lista de dispositivos registrados.
    }
    else{
        Serial.println("Lista de dispositivos llena");
    }
}

void ModeChangeRequest(bool toTag){

    if(toTag){
        Serial.println("Dispositivo cambiado a TAG");
        DW1000Ranging.startAsTag(DEVICE_ADDR,DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
    }
}
void MostrarDatos(){

    Serial.println("--------- NUEVA MEDIDA ---------");
    
    for (int i = 0; i < numDispositivos ; i++){ 
        Serial.print(" Desde: ");
        Serial.print(medidas[i].shortAddr,HEX);
        Serial.print("\t Distancia: ");
        Serial.print(medidas[i].distancia);
        Serial.print(" m \t RX power: ");
        Serial.print(medidas[i].rxPower);
        Serial.println(" dBm");
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

void loop(){

    DW1000Ranging.loop();
    current_time = millis();
    if(current_time - last_print >= refresh_time){

        MostrarDatos();
        last_print = millis();
    }
}