# Trabajo Fin de Grado Jaime Pérez: Ultra Wide Band

Códigos empleados para la elaboración del Trabajo de Fin de Grado. 

<p align="center">
 <img src="https://cdn.worldvectorlogo.com/logos/arduino-1.svg" alt="arduino" width="60" height="60"/> <img src="https://github.com/jimmyperezp/TFG_UWB/blob/main/logo%20UWB.png" alt="arduino" width="60" height="60"/>
<img src="https://github.com/jimmyperezp/Programacion_de_sistemas/blob/main/logo%20escuela.png" alt="logo industriales" width="300" height="100"/> 


### Índice:
1. [Objetivo del Proyecto](#objetivo)
2. [Hardware Utilizado](#hardware)
3. [Contenido del repositorio](#contenido-del-repositorio)
4. [Configuracion Arduino IDE](#configuracion-arduino)
5. [Estado del repositorio](#estado-del-repositorio)

<br></br>

## Objetivo 

Este proyecto de fin de grado consiste en el estudio de la tecnología de **Banda Ultra Ancha** (Ultra Wide Band, o **UWB**) para su implementación en sistemas ferroviarios. Con su uso, se pretende controlar en todo momento la integridad del tren que lo lleve, así como la comunicación con otros sensores y elementos de medida del entorno para caracterizar e identificar posibles peligros. 

Dentro de este repositorio se encuentran los códigos (librería incluida) utilizados para el desarrollo de la fase inicial del proyecto.  

Esta fase inicial consiste en la comunicación y uso de varias placas ESP32 + UWB. Se verá desde los primeros pasos para configurarlas y utilizarlas, hasta la comunicación y uso de un sistema en el que se integren más de un elemento de cada tipo. 


 

<br></br>

## Conceptos previos: Banda Ultra Ancha (UWB)

Para comenzar, es importante tener claros una serie de conceptos relacionados con la tecnología que se va a utilizar: 

**1: Cómo calcular distancias: TWR**  

Para calcular la distancia que existe entre 2 chips UWB, estos utilizan el protocolo de comunicación 

<img src="https://cdn.sewio.net/wp-content/uploads/2016/04/TWR-Scheme.jpg" alt="Imagen explicativa TWR" width="400" height="400"/>


Esto consiste en que uno de los dispositivos comienza la comunicación haciendo *polling*, mientras que el otro se limita a responder.  
1. El que hace polling envía sin parar un mensaje con el instante temporal en el que lo ha enviado, esperando recibir respuesta. 
2. Si logra conectar con otro chip (que esté configurado como respondedor), este le devolverá el instante en el que recibió el mensaje original y el instante en el que ha enviado su respuesta.  
3. El último paso es que el iniciador vuelve a enviarle al respondedor el instante en el que recibió su mensaje, y el instante de su mensaje final.  

De este modo, el dispositivo configurado como respondedor conoce ya todos los instantes temporales en los que se ha enviado cada mensaje. Con ellos, puede calcular haciendo una simple resta el tiempo que dichos mensajes han estado en el aire. 
Finalmente, haciendo una simple división usando la velocidad a la que se envían los datos, puede calcular la distancia entre ambos dispositivos. 

Este proceso es el conocido como Two-Way-Ranging.

**2: Nomenclatura:** 
Teniendo en cuenta lo recién visto, en la librería se han declarado dos modos de operación para cada dispositivo: 

1. **Iniciador**: Normalmente, este suele ser el Tag. Sin embargo, puede que las necesidades concretas del sistema sean distintas, así que he optado por este nombre.   
El dispositivo iniciador es el que comienza el proceso de *polling*

2. **Respondedor**: Lógicamente, es el que responde al poll inicial. Es en este dispositivo donde se realiza el cálculo de las distancias. 

En la librería utilizada, el respondedor le envía un report al iniciador con el cálculo que ha realizado, de tal manera que ambos dispositivos tengan acceso a dicho valor, aunque sea el respondedor el encargado de calcularlo. 


<br></br>

## Hardware 

Las placas utilizadas son las desarrolladas por MakerFabs: [Placa ESP32-UWB MakerFabs](https://www.makerfabs.com/esp32-uwb-ultra-wideband.html?srsltid=AfmBOoptL7z67ua57v7tP1AYSjEUQVG0_JfwDDH6NKWy50RSJLR1hWZG)


![Imagen Placa ESP32-UWB](https://github.com/Makerfabs/Makerfabs-ESP32-UWB/blob/main/md_pic/front.jpg?raw=true)

El microcontrolador de estas placas es una ESP32
- [ESP32 Wroom Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_en.pdf)


Además, llevan el chip DW1000 para la comunicación UWB: 

- [DW1000 Datasheet](https://www.qorvo.com/products/d/da007946)
- [DW1000 User Manual](https://www.decawave.com/sites/default/files/resources/dw1000_user_manual_2.11.pdf)
- [DW1000 Antenna Delay Calibration](https://www.decawave.com/wp-content/uploads/2018/10/APS014_Antennna-Delay-Calibration_V1.2.pdf)


<br></br>

## Contenido del Repositorio: 
0. Librería actualizada para el desarrollo del TFG [Librería DW1000 actualizada por mí](00%20-%20Libreria%20TFG%20Jaime%20Perez)
    - Dentro de la carpeta de la librería, hay un README explicando todos los cambios realizados sobre la versión anterior. 
    - Los **ejemplos** que incluye la librería son los mismos que se van a detallar en las carpetas de este repositorio. La diferencia está en que en estas carpetas, incluyo un readme explicando cada uno de ellos.
    - Además, los ejemplos de dentro de la librería están comentados y documentados en inglés, para su posterior publicación en GitHub.
    - El procedimiento recomendado es: descargarse la librería para poder acceder a los ejemplos desde  el Arduino IDE. A la hora de trabajar con cada uno de ellos, acceder a su readme y entender su código desde las carpetas posteriores de este repositorio.


1. Configuración de la ESP32 para usar en el Arduino IDE. Ver paso a paso: [configurar el Arduino IDE](#configuracion-arduino)

2. Medir distancias entre 1 tag y 1 anchor:  [Distancia entre 1 Tag y 1 Anchor](Medir%20distancias)
    - En esta carpeta están los códigos utilizados para medir la distancia entre un tag y un anchor. Dentro de ella, hay un readme explicando el procedimiento. 

3. Plottear la posición entre 1 tag y 2 anchors:  [Posicionamiento 2D](03%20-%20Posicionamiento%202D)
    - Este ejemplo consiste en mostrar mediante un programa de python la posición relativa entre 2 anchors y 1 tag. Utiliza el módulo WiFi de la ESP32 para enviar datos al PC. 

4. Centralizar los datos en uno de los anchors [Centralizar datos](04%20-%20Centralizar%20Datos)
    - Con los códigos de este ejemplo se envían los datos de las posiciones relativas de todos los dispositivos del sistema entre sí (tanto anchors como tags) a un anchor "maestro". Ahí se recopilan todas las distancias y potencias y se muestran por pantalla.
<br></br>

## Configuracion Arduino 

Hay que preparar el Arduino IDE para trabajar con la placa ESP32+UWB

Los pasos a seguir son los siguientes:

1 - **Instalar las boards de ESP32**. Hay que ir a tools >> Board >> Board Manager >>  Buscar "ESP32". Descargar las placas de espressif Systems.

2 - Añadir las **librerías** necesarias para esta placa:
  - 2.1 - Descargar la carpeta DW1000 de este repositorio. Después, ir a Sketch >> Include Library >> Add .Zip library >> Y seleccionar la carpeta recién descargada.
  - 2.2 - Añadir las librerías Adafruit_SSD1306. Ir a Sketch >> Include Library >> Manage Libraries >> Buscar Adafruit_SSD1306 e instalar.

3 - Seleccionar la **placa DOIT ESP32 DevKit**


<br></br>

## Estado del repositorio
El repositorio está **activo**. Iré actualizándolo y subiendo los códigos empleados a medida que se pruebe su funcionamiento

<br></br>

-------------
Autor: Jaime Pérez  
Última modificación: 08/09/2025
