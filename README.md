# Trabajo Fin de Grado Jaime Pérez: Ulta Wide Band

Códigos empleados para la elaboración del Trabajo de Fin de Grado. 

 <img src="https://cdn.worldvectorlogo.com/logos/arduino-1.svg" alt="arduino" width="60" height="60"/> <img src="https://github.com/jimmyperezp/TFG_UWB/blob/main/logo%20UWB.png" alt="arduino" width="60" height="60"/>
<img src="https://github.com/jimmyperezp/Programacion_de_sistemas/blob/main/logo%20escuela.png" alt="logo industriales" width="300" height="100"/> 

<br></br>
## Contenido: 

1. Configuración de la ESP32. Ver pasos en la carpeta "Arduino IDE" [configurar el Arduino IDE](#ConfiguracionArduino )

2. Medir distancias entre 2 placas 

<br></br>
<br></br>
<br></br>
<br></br>
<br></br>
<br></br>

## Configuracion Arduino 

Hay que preparar el Arduino IDE para trabajar con la placa ESP32+UWB

Los pasos a seguir son los siguientes:

1 - Instalar las boards de ESP32. Hay que ir a tools >> Board >> Board Manager >>  Buscar "ESP32". Descargar las placas de espressif Systems.

2 - Añadir las librerías necesarias para esta placa:
  2.1 - Descargar la carpeta DW1000 de este repositorio. Después, ir a Sketch >> Include Library >> Add .Zip library >> Y seleccionar la carpeta recién descargada.
  2.2 - Añadir las librerías Adafruit_SSD1306. Ir a Sketch >> Include Library >> Manage Libraries >> Buscar Adafruit_SSD1306 e instalar.

3 - Seleccionar la placa DOIT ESP32 DevKit


<br></br>
## Estado del repositorio
El repositorio está activo. Iré actualizándolo y subiendo los códigos empleados a medida que se pruebe su funcionamiento


-------------
Autor: Jaime Pérez.

Última modificación: 27/08/2025