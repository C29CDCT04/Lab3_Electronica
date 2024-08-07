# Lab3_Electronica_

Laboratorio 3 - Interrupciones y Temporizador

En este laboratorio, se utilizó el microcontrolador ESP32.

Parte 1: Se diseñó e implementó un contador binario de 4 bits, donde cada incremento se realizaba mediante un botón (B1) y cada decremento mediante otro botón (B2), utilizando interrupciones para los botones.

Parte 2: Se diseñó e implementó un contador binario de 4 bits que incrementaba automáticamente cada 250 milisegundos, utilizando un temporizador.

Parte 3: Se implementó una funcionalidad en la que, cuando el contador del temporizador era igual al contador de los botones, se cambiaba el estado de un pin como indicador y se reiniciaba el contador del temporizador. Esto permitía variar la velocidad de encendido y apagado del indicador utilizando los botones.

Parte 4: Se utilizó un sensor capacitivo para implementar un botón (B3) que reiniciaba el contador del temporizador cada vez que se presionaba.

Todas las partes de este laboratorio funcionan simultáneamente.
