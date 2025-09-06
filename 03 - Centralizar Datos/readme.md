# Centralización de los datos

<img src="https://cdn.worldvectorlogo.com/logos/arduino-1.svg" alt="arduino" width="40" height="40"/>

### Objetivo

Este ejemplo trata de centralizar todos los datos medidos del sistema en un único anchor.  
De esta manera, tener acceso a un único dispositivo (al master anchor) nos permitirá conocer la situación del sistema completo. 

### Funcionamiento: 

Tal y como estaba diseñada la anterior versión de la librería, eran los tags los encargados de iniciar la comunicación con los anchors (los tags son los que comienzan con el *polling*).  
Para centralizar todos los datos en un anchor maestra, era necesario que dicho dispositivo se encargara de lanzar las comunicaciones, ordenando a las anclas esclavas a cambiar su modo de funcionamiento y a pedirles que le envíen sus datos.  
Era, por tanto, necesario definir los anchors como tags (para que comiencen con el *polling*), y los tags como anchors.  
Esto era motivo de confusión y dificultad al seguir el código. 
 
El **proceso de recopilación de todos los datos desde el ancla maestra** es el siguiente: 
 
1. El ancla maestra comienza a hacer polling con el resto de dispositivos que existan en el sistema. Para ello, todos los demás deben estar inicializados como receptores. Es decir, al inicio, solo el ancla maestra debe funcionar como iniciador.

2. Según se realizan las mediciones (usando el mismo procedimiento que se explicó en el ejemplo 1: [Medir Distancias]((Medir%20distancias))), se almacenan los datos en un struct que se irá actualizando con las siguientes medidas.

3. Tras almacenar estos resultados, el ancla maestra ya conoce su distancia hacia todos los dispositivos del sistema. 
El siguiente paso es, por tanto, que el resto de los dispositivos calculen sus distancias entre sí. 
 
4. Para hacerlo, asumiendo que el sistema se compone de 1 tag, 1 ancla maestra y el resto son anclas esclavas, es necesario pedirle a cada una de las anclas esclavas que cambien su modo de funcionamiento de receptores a iniciadores (para que puedan hacer *polling*).

5. Una vez han cambiado de modo de operación, podrán medir su distancia con respecto al tag. Posteriormente, registrarán dicha distancia en un struct como el utilizado en el ancla maestra.

6. Finalmente, cuando todas las anclas esclavas han realizado sus mediciones, el ancla maestra debe lanzar un mensaje para solicitar los datos medidos. 

7. Los slave anchors devolverán sus structs de medidas correctamente codificados para que el maestro pueda entender los mensajes. 

8. El último paso será mostrar por pantalla todas las mediciones, indicando el dispositivo de origen, el de destino, la distancia entre ellos, y la potencia de la comunicación realizada. 

 
### Código I: Adaptaciones de la librería

El código de este ejemplo fue el responsable de la modificación de las funciones y definiciones de la librería utilizada hasta entonces. 

Como ya se ha visto antes, el hecho de declarar los tags como inciadores y anchors como receptores era motivo de confusión, por lo que se han sustituido los tipos de dispositivos de TAG y ANCHOR a INITIATOR y RESPONDER

```C++
//sketch type (responder or Initiator)
#define INITIATOR 0
#define RESPONDER 1

```
Y además, se han modificado los métodos de inicialización de los dispositivos. en lugar de startAsAnchor y startAsTag, ahora son: 

```C++
void startAsResponder (...);
void startAsInitiatior(...);

```

Para el control del flujo de los datos, desde el cambio en modo de funcionamiento hasta el envío de los datos codificados, se han realizado las siguientes mejoras:

Primero, se han declarado los nuevos tipos de mensajes que se pueden enviar entre dispositivos: 

```C++

//messages sent for data and flow control
#define MODE_SWITCH 6 
#define REQUEST_DATA 7 
#define DATA_REPORT 8 

```
Después, se han creado los métodos para enviar dichos mensajes: 

```C
void DW1000RangingClass::transmitModeSwitch(bool toInitiator, DW1000Device* device){ ...};

void DW1000RangingClass::transmitRequestData  (DW1000Device* device) {...}

void DW1000RangingClass::transmitDataReport(Medida* medidas, int numMedidas, DW1000Device* device) {...}

```

Y, por último, se han configurado los callbacks apropiados para lanzarse cuando se detecta la recepción de alguno de estos mensajes.  
En el programa que se sube al anchor (CD_anchor.ino), se han enlazado esos callbacks a las funciones que gestionan la información recibida. Estas funciones son:

```C
void DataReport();
void ModeChangeRequest();
void DataRequest();
```

Todas ellas están comentadas dentro del código. Ver su funcionamiento ahí.



**IMPORTANTE**: Todas estas funciones y nuevos cambios de la librería están explicados y detallados en la [Documentación de los cambios en la librería]() 





### Hardware

El código está adaptado para su escalabilidad, pero las pruebas se han realizado utilizando **2 Anchors y 1 Tag**


### Mejoras pendientes: TO-DO list:

**Problema #1**: Escalabilidad. Unicast frente a broadcasting.
- Actualmente, solo hay un anchor esclavo, por lo que basta con hacer broadcast desde el maestro para solicitar su cambio de modo de funcionamiento. Sin embargo, si escalamos el sistema, será necesario cambiar los slave anchors uno por uno, dándoles suficiente tiempo como para que realicen todas sus medidas y las guarden correctamente. 

- Al hacer el data report sucede lo mismo. Al solicitarlo por broadcast, todos los slave anchors enviarán sus resultados a la vez, por lo que el maestro puede tener problemas de interferencia. 

**Solución**: Si aumenta el número de slave anchors, hay que implementar una manera de enviar los mensajes de switch_mode, data_request y data_report haciendo unicast. Hay que tener en cuenta que al cambiar de modo y enviar los datos, las placas tienen un pequeño delay, por lo que hay que darles tiempo suficiente para realizar y mandar sus medidas correctamente.


<br><br>
------------
Autor: Jaime Pérez  
Última modificación: 2/9/2025