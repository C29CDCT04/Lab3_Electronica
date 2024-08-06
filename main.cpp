/*Universidad Del Valle de Guatemala
Facultad de Ingeniería
Departamento de Electrónica, Mecatrónica y Biomédica
Electrónica Digital 2

Carlos Daniel Camacho Tista - 22690
Susan Daniela Guevara Catalán - 22519

Laboratorio No. 3 - Interrupciones y temporizadores

Contador (Botón) binario de 4 bits que se puede incrementar con un botón o decrementar con otro utilizando interrupciones.
Contador (Timer) binario de 4 bits en el que cada incremento se realizará cada 250us, utilizando un temporizador.
En el momento que el contador del timer sea igual al contador de los botones, se cambia el estado de un pin como indicador
(Alarma) y reinicia el contador del timer. 
Se puede reiniciar el contador del timer presionando un botón (B3)*/

/**********Importación de librerías**********/

#include <Arduino.h>
#include "driver/timer.h" // Incluye las definiciones para el temporizador de hardware del ESP32

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

/**********Variables globales**********/

// Pines para los LEDs para el contador temporizador
const int ledt1 = 15; 
const int ledt2 = 2;
const int ledt3 = 4;
const int ledt4 = 5;

// Pines para los LEDs para el contador botón
#define Led_1 18
#define Led_2 19
#define Led_3 21
#define Led_4 23

// Pin para el LED de la alarma
const int Alarma = 13; 

//Variable para el contador del botón de incremento y decremento
uint8_t count = 1;

// Pin para el sensor capacitivo
const int sensorPin = 33;

// Umbral para detectar una pulsación del sensor capacitivo
int touchThreshold = 40; 

// Estructura del boton de incremento y decremento
struct Button{
  const uint8_t PIN;
  volatile uint32_t numberKeyPresses;
  bool pressed;
};

Button Suma ={12, 0, false};
Button Resta ={27, 0, false};

// Estructura del boton de reset
struct CapacitiveSensor{
  const uint8_t PIN;
  int touchValue;
  bool pressed;
};

CapacitiveSensor Reset = {sensorPin, 0, false};

// Variable para el temporizador
volatile int tempo = 0; 

// Puntero al temporizador
hw_timer_t *timer = NULL; 

/**********Prototipo de funciones**********/

//Contador (Timer) binario de 4 bits
void IRAM_ATTR onTimer(void);

//Contador (Botón) binario de 4 bits que se puede incrementar con un botón
void IRAM_ATTR BTN_SUMA_ISR(void);

//Contador (Botón) binario de 4 bits que se puede decrementar con un botón
void IRAM_ATTR BTN_RESTA_ISR(void);

/*********Configuración de entradas y salidas*********/
void setup() {

  // Configuración los pines de los LEDs como salidas
  pinMode(ledt1, OUTPUT);
  pinMode(ledt2, OUTPUT);
  pinMode(ledt3, OUTPUT);
  pinMode(ledt4, OUTPUT);
  pinMode(Led_1, OUTPUT);
  pinMode(Led_2, OUTPUT);
  pinMode(Led_3, OUTPUT);
  pinMode(Led_4, OUTPUT);
  pinMode(Alarma, OUTPUT);

  // Configuración del contador del botón de incremento
  Serial.begin(115200);
  pinMode(Suma.PIN,INPUT_PULLUP);
  attachInterrupt(Suma.PIN, BTN_SUMA_ISR, FALLING);

  // Configuración del contador del botón de decremento
  Serial.begin(115200);
  pinMode(Resta.PIN,INPUT_PULLUP);
  attachInterrupt(Resta.PIN, BTN_RESTA_ISR, FALLING);

  // Configuración del temporizador
  timer = timerBegin(0, 80, true); // Inicializar el temporizador 0, con prescaler 80, y contar hacia arriba
  timerAttachInterrupt(timer, &onTimer, true); // Adjuntar la interrupción al temporizador
  timerAlarmWrite(timer, 250000, true); // Configurar la alarma para 250 us (250,000 microsegundos)
  timerAlarmEnable(timer); // Habilitar la alarma del temporizador
}

/*********Bucle infinito*********/
void loop() {
//Cuando se presiona el botón Suma, se incrementa un número binario  
  if(Suma.pressed){
    //Asegura que lee cuando se presiona el botón
    Serial.printf("Button add has been pressed %u times\n",Suma.numberKeyPresses);
    count = (count + 1) % 16; // Incrementa y vuelve a 0 después de 15
    digitalWrite(Led_1, count & 0x01);
    digitalWrite(Led_2, (count >> 1) & 0x01);
    digitalWrite(Led_3, (count >> 2) & 0x01);
    digitalWrite(Led_4, (count >> 3) & 0x01);
    Suma.pressed=false;
  }

//Cuando se presiona el botón Resta, se decrementa un número binario
  if(Resta.pressed){
    //Asegura que lee cuando se presiona el botón
    Serial.printf("Button rest has been pressed %u times\n",Resta.numberKeyPresses);
    count = (count - 1) % 16; // Decrementa y vuelve a 15 después de 0
    digitalWrite(Led_1, count & 0x01);
    digitalWrite(Led_2, (count >> 1) & 0x01);
    digitalWrite(Led_3, (count >> 2) & 0x01);
    digitalWrite(Led_4, (count >> 3) & 0x01);
    Resta.pressed=false;
  }

// Cuando se presiona el botón Reset, se reinicia el contador del temporizador
  // Leer el valor del sensor capacitivo
  Reset.touchValue = touchRead(Reset.PIN);

  // Si se detecta una pulsación en el sensor capacitivo, reiniciar el temporizador
  if (Reset.touchValue < touchThreshold) {
    Serial.printf("Capacitive sensor touched, value: %d\n", Reset.touchValue);
    timerAlarmDisable(timer); // Deshabilitar el temporizador
    tempo = 0; // Reiniciar el contador del temporizador
    timerAlarmEnable(timer); // Habilitar de nuevo el temporizador
  }
}

/*Definición de funciones*/

// Interrupción por el botón que incrementa
void IRAM_ATTR BTN_SUMA_ISR(){
  portENTER_CRITICAL_ISR(&mux);
    Suma.numberKeyPresses++;
    Suma.pressed=true;
  portEXIT_CRITICAL_ISR(&mux);
}

// Interrupción por el botón que decrementa
void IRAM_ATTR BTN_RESTA_ISR(){
  portENTER_CRITICAL_ISR(&mux);
    Resta.numberKeyPresses++;
    Resta.pressed=true;
  portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR onTimer() {
  tempo = (tempo + 1) % 16; // Incrementar el contador y reiniciar a 0 después de 15

  // Mostrar el valor del contador en los pines digitales
  digitalWrite(ledt1, bitRead(tempo, 0));
  digitalWrite(ledt2, bitRead(tempo, 1));
  digitalWrite(ledt3, bitRead(tempo, 2));
  digitalWrite(ledt4, bitRead(tempo, 3));

  // Comparar el contador del temporizador con el contador de los botones
  if (tempo == count) {
    digitalWrite(Alarma, HIGH); // Activar la alarma
    timerAlarmDisable(timer); // Deshabilitar el temporizador
    tempo = 0; // Reiniciar el contador del temporizador
    timerAlarmEnable(timer); // Habilitar de nuevo el temporizador
  } else {
    digitalWrite(Alarma, LOW); // Desactivar la alarma si no son iguales
  }
}

