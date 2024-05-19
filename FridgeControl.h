#ifndef FRIDGECONTROL_H
#define FRIDGECONTROL_H

#include <PID_v1.h>

#ifndef TEST_MODE
#include <max6675.h>
#define RELAY_PIN 2 // Pin digital al que está conectado el relé
#define THERMOCOUPLE_CLK 3 // Pin digital al que está conectado el CLK del termopar
#define THERMOCOUPLE_CS 4 // Pin digital al que está conectado el CS del termopar
#define THERMOCOUPLE_DO 5 // Pin digital al que está conectado el DO del termopar
MAX6675 thermocouple(THERMOCOUPLE_CLK, THERMOCOUPLE_CS, THERMOCOUPLE_DO);
#define TIME_SCALE 1
#else
#include "MockFridge.h"
MockFridge fridge;
#define TIME_SCALE 300
#endif

double setpoint = 4.0; // Temperatura deseada en grados Celsius
double input, output;

// Variables para el PID
double kp = 0.6, ki = 0.15, kd = 0.3; // Valores ajustados para una respuesta más lenta
PID myPID(&input, &output, &setpoint, kp, ki, kd, REVERSE);

unsigned long interval = 30 * 60 * 1000 / TIME_SCALE; // Intervalo de 30 minutos (en milisegundos)
unsigned long maxRunTime = 4 * 60 * 60 * 1000 / TIME_SCALE; // Máximo tiempo de funcionamiento de 4 horas (en milisegundos)
unsigned long compressorStateChangeMillis = interval;

bool compressorState = false; // Estado inicial del compresor

inline void setupFridgeControl() {
#ifndef TEST_MODE
  pinMode(RELAY_PIN, OUTPUT);
#endif
  pinMode(LED_BUILTIN, OUTPUT);
  myPID.SetSampleTime(60000 / TIME_SCALE); // Frecuencia de muestreo de 60 segundos para el PID
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(-1, 1); // Establecer límites de salida del PID
  Serial.begin(9600);
}

inline void loopFridgeControl() {
  unsigned long currentMillis = millis();

#ifdef TEST_MODE
  fridge.updateTemperature(currentMillis * TIME_SCALE);
  input = fridge.getTemperature();
#else
  // Leer la temperatura
  double temperature = thermocouple.readCelsius();
  if (!isnan(temperature)) {
    input = temperature;
  }
#endif

  // Calcular el PID
  if (myPID.Compute()) {
    // Cambiar el estado del compresor si ha pasado suficiente tiempo desde el último cambio de estado
    if ((currentMillis - compressorStateChangeMillis >= interval) ||
        (compressorState && (currentMillis - compressorStateChangeMillis >= maxRunTime))) {
      if (output > 0.75 && !compressorState) { // Si el compresor está apagado, ser más exigente con el control PID, y solo arrancar si hay fuerte evidencia de que es necesario
#ifdef TEST_MODE
        compressorState = true;
#else
        digitalWrite(RELAY_PIN, HIGH); // Encender el compresor
        compressorState = true;
#endif
        compressorStateChangeMillis = currentMillis;
      } else if ((output <= 0 || (compressorState && (currentMillis - compressorStateChangeMillis >= maxRunTime))) && compressorState) {
#ifdef TEST_MODE
        compressorState = false;
#else
        digitalWrite(RELAY_PIN, LOW); // Apagar el compresor
        compressorState = false;
#endif
        compressorStateChangeMillis = currentMillis;
      }
    }

    // Enviar valores por Serial Plotter
    Serial.print(input, 1);
    Serial.print(",");
    Serial.print(output, 1);
    Serial.print(",");
    Serial.print(setpoint, 1);
    Serial.print(",");
    Serial.println(compressorState ? 1 : 0);
  }

#ifdef TEST_MODE
  fridge.setCompressorState(compressorState);
#endif

  // Controlar el LED integrado
  double tempDifference = abs(setpoint - input);
  unsigned int blinkInterval = map(tempDifference, 0, 30, 750, 100); // Mapea la diferencia a un intervalo de parpadeo
  static unsigned long lastBlinkTime = 0;
  if (currentMillis - lastBlinkTime >= blinkInterval) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // Cambiar el estado del LED
    lastBlinkTime = currentMillis;
  }
}

#endif // FRIDGECONTROL_H
