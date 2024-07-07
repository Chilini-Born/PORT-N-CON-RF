#include <RCSwitch.h>

#define CODIGO 16242856
#define INTERVALO 2000
long dato;
bool estado = false;
bool codigoValido = false;
unsigned long tiempoAnterior;

const int pinBotonModo = 3;     // El número del pin del botón
const int pinLedModo = 2;       // El número del pin del LED
int estadoBotonModo = 0;        // Variable para leer el estado del botón
int estadoBoton = 0;            // Variable para leer el estado del botón
bool modoManual = true;         // Empieza en modo manual
const int pinBoton = 4;         // El número del pin del botón apertura/cierre
const int pinLedPorton = 6;     // El número del pin del LED de dirección
const int pinLedMotor = 7;      // El número del pin del LED de activación del motor
const int pinFCC = 8;           // El número del pin del interruptor FCC
const int pinFCA = 5;           // El número del pin del interruptor FCA
int ultimoEstadoBotonModo = 0;  // Variable para el último estado del botón
int ultimoEstadoBoton = 0;      // Variable para el último estado del botón
bool ledEncendido = false;      // Indica si algún LED está encendido
int estadoGeneral = 3;          // Estado general del portón
int estadoGeneralTemp = 3;      // Estado general del portón
// 0 cerrado
// 1 abierto
// 2 abriéndose
// 3 cerrándose
// 4 detención en apertura
// 5 detención en cierre
unsigned long tiempoInicio = 0;           // Almacena el tiempo en el que el LED fue encendido
const unsigned long duracionLed = 2000;   // Duración del LED encendido en milisegundos
int banderaAntiRebote = 0;

void setup() {
  Serial.begin(9600);
  pinMode(pinBotonModo, INPUT_PULLUP);
  pinMode(pinLedModo, OUTPUT);
  pinMode(pinBoton, INPUT_PULLUP);
  pinMode(pinLedPorton, OUTPUT);
  pinMode(pinLedMotor, OUTPUT);
  pinMode(pinFCC, INPUT_PULLUP);  // FCC inicialmente HIGH
  pinMode(pinFCA, INPUT_PULLUP);  // FCA inicialmente HIGH
}

void loop() {  
  // Leer el estado del botón:
  estadoBotonModo = digitalRead(pinBotonModo);

  // Comprobar si el botón está presionado y si el estado ha cambiado, alternar el modo
  if (estadoBotonModo == LOW && banderaAntiRebote == 0) {
    modoManual = !modoManual;
    banderaAntiRebote = 1;
  }
  if (estadoBotonModo == HIGH) {
    banderaAntiRebote = 0;
  }

  if (modoManual) {
    digitalWrite(pinLedModo, LOW);
    funcionModoManual();
  } else {
    digitalWrite(pinLedModo, HIGH);
    funcionModoTemporizado();
  }

  // Guardar el estado actual como el último estado, para la próxima vez
  ultimoEstadoBotonModo = estadoBotonModo;
}

void funcionModoTemporizado() {
  // Leer el estado del botón
  if(estadoGeneral == 4)
      {
        digitalWrite(pinLedMotor, HIGH);
        Serial.println("Cerrandose...");
        estadoGeneral = 3;
      }
  estadoBoton = digitalRead(pinBoton);
  
  if (estadoBoton != ultimoEstadoBoton || dato==CODIGO) {
    if (estadoBoton == HIGH || dato==CODIGO) {
      switch (estadoGeneral) {
        case 0: // Cerrado
          digitalWrite(pinLedPorton, HIGH);
          delay(10);
          digitalWrite(pinLedMotor, HIGH);
          Serial.println("Abriendose...");
          estadoGeneral = 2;
          break;
        case 1: // Abierto
          digitalWrite(pinLedMotor, HIGH);
          Serial.println("Cerrandose...");
          estadoGeneral = 3;
          break;
        case 2: // Abriéndose
          digitalWrite(pinLedPorton, LOW);
          delay(10);
          digitalWrite(pinLedMotor, LOW);
          Serial.println("Detenido en apertura...");
          estadoGeneral = 1;
          break;
        case 3: // Cerrándose
          digitalWrite(pinLedPorton, LOW);
          delay(10);
          digitalWrite(pinLedMotor, LOW);
          Serial.println("Detenido en cierre...");
          estadoGeneral = 5;
          break;
        /*case 4: // Detenido en apertura
          digitalWrite(pinLedMotor, HIGH);
          Serial.println("Cerrandose...");
          estadoGeneral = 3;
          break;*/
        case 5: // Detenido en cierre
          digitalWrite(pinLedPorton, HIGH);
          delay(10);
          digitalWrite(pinLedMotor, HIGH);
          Serial.println("Abriendose...");
          estadoGeneral = 2;
          break;
      }
      
    }
	
    // Retraso para evitar el rebote
    delay(50);
  }
  
  // Verificar interruptores de fin de carrera
  if (estadoGeneral == 2 && digitalRead(pinFCA) == HIGH) {
    Serial.println("Abierto.");
    digitalWrite(pinLedPorton, LOW);
    delay(10);
    digitalWrite(pinLedMotor, LOW);
    Serial.println("Espera programada de 2 segundos");
    delay(duracionLed);
    Serial.println("Espera terminada");
    estadoGeneral = 4;
    //estadoBoton != estadoBoton;
  }
  
  if (estadoGeneral == 3 && digitalRead(pinFCC) == HIGH) {
    Serial.println("Cerrado.");
    digitalWrite(pinLedMotor, LOW);
    estadoGeneral = 0;
  }

  // Guardar el estado actual como el último estado, para la próxima vez
  ultimoEstadoBoton = estadoBoton;
}

void funcionModoManual() {
  // Leer el estado del botón
  estadoBoton = digitalRead(pinBoton);
  
  if (estadoBoton != ultimoEstadoBoton || dato == CODIGO) {
    if (estadoBoton == HIGH || dato == CODIGO) {
      switch (estadoGeneral) {
        case 0: // Cerrado
          digitalWrite(pinLedPorton, HIGH);
          delay(10);
          digitalWrite(pinLedMotor, HIGH);
          Serial.println("Abriendose...");
          estadoGeneral = 2;
          break;
        case 1: // Abierto
          digitalWrite(pinLedMotor, HIGH);
          Serial.println("Cerrandose...");
          estadoGeneral = 3;
          break;
        case 2: // Abriéndose
          digitalWrite(pinLedPorton, LOW);
          delay(10);
          digitalWrite(pinLedMotor, LOW);
          Serial.println("Detenido en apertura...");
          estadoGeneral = 4;
          break;
        case 3: // Cerrándose
          digitalWrite(pinLedMotor, LOW);
          Serial.println("Detenido en cierre...");
          estadoGeneral = 5;
          break;
        case 4: // Detenido en apertura
          digitalWrite(pinLedMotor, HIGH);
          Serial.println("Cerrandose...");
          estadoGeneral = 3;
          break;
        case 5: // Detenido en cierre
          digitalWrite(pinLedPorton, HIGH);
          delay(10);
          digitalWrite(pinLedMotor, HIGH);
          Serial.println("Abriendose...");
          estadoGeneral = 2;
          break;
      }
    }

    // Retraso para evitar el rebote
    delay(50);
  }
  
  // Verificar interruptores de fin de carrera
  if (estadoGeneral == 2 && digitalRead(pinFCA) == HIGH) {
    Serial.println("Abierto.");
    digitalWrite(pinLedPorton, LOW);
    estadoGeneral = 1;
  }
  
  if (estadoGeneral == 3 && digitalRead(pinFCC) == HIGH) {
    Serial.println("Cerrado.");
    digitalWrite(pinLedMotor, LOW);
    estadoGeneral = 0;
  }

  // Guardar el estado actual como el último estado, para la próxima vez
  ultimoEstadoBoton = estadoBoton;
}
