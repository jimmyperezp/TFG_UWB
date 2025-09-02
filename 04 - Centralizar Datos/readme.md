# Centralización de los datos

<img src="https://cdn.worldvectorlogo.com/logos/arduino-1.svg" alt="arduino" width="40" height="40"/>

### Objetivo

Este ejemplo trata de centralizar todos los datos medidos del sistema en un único anchor.  
De esta manera, tener acceso a un único dispositivo (al master anchor) nos permitirá conocer la situación del sistema completo. 

### Funcionamiento: 

En los ejemplos vistos hasta ahora, tal y como está diseñada la librería, son los tags los que inician la comunicación con los anchors (son los tags los que comienzan con el *polling*).  
El primer cambio es, por tanto, definir un anchor maestra que sea la que comienza las comunicaciones (en realidad, según las funciones de la librería, este paso consiste en declarar los anchors como tags y viceversa).  
El proceso de **recopilación de todos los datos** es el siguiente: 
1. El ancla maestra comienza a hacer polling con el resto de dispositivos que existan en el sistema. Para ello, todos los demás deben estar inicializados (según la librería usada) como anchors

2. Según se realizan las mediciones (usando el mismo procedimiento que se explicó en el ejemplo 1: [Medir Distancias]((Medir%20distancias))), se almacenan los datos en un struct que se irá actualizando con las siguientes medidas.

3. Una vez calculada la distancia hasta cada elemento, ahora faltan las distancias entre dichos dispositivos entre sí. Para hacerlo, el ancla maestra le manda una señal a las otras anclas (una a una) de que pasen a actuar como iniciadoras de medición (que se empiecen a comportar como un tag). 

4. Tras el cambio en la configuración, comenzarán a hacer polling. Medirán la distancia entre sí mismas con el resto de dispositivos siguiendo el mismo procedimiento. 

5. El último paso es enviar esta información de vuelta al ancla maestra. 


### Hardware

El código está adaptado para su escalabilidad, pero las pruebas se han realizado utilizando **2 Anchors y 1 Tag**



<br><br>
------------
Autor: Jaime Pérez  
Última modificación: 2/9/2025