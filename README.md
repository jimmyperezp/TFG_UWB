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

Es importante tener clara la **nomenclatura** utilizada en el trabajo: 

1. **ANCHOR**: Los Anchors son las anclas. Son las placas que se quedan "fijas" (o al menos, cuya posición es relativamente conocida)
2. **TAG**: Son las placas móviles. 

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
Autor: Jaime Pérez.

Última modificación: 29/08/2025
