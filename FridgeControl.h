#ifndef FRIDGECONTROL_H
#define FRIDGECONTROL_H

#include <PID_v1.h>
#include <max6675.h>
#include "TemperatureFilter.h"
#include "Compressor.h"

#define RELAY_PIN 2 // Pin digital al que está conectado el relé
#define THERMOCOUPLE_CLK 3 // Pin digital al que está conectado el CLK del termopar
#define THERMOCOUPLE_CS 4 // Pin digital al que está conectado el CS del termopar
#define THERMOCOUPLE_DO 5 // Pin digital al que está conectado el DO del termopar

MAX6675 thermocouple(THERMOCOUPLE_CLK, THERMOCOUPLE_CS, THERMOCOUPLE_DO);

#define COMPENSATION -14.0

double setpoint = 4.0; // Temperatura deseada en grados Celsius
double input, output, lastPIDOutput;

// Variables para el PID
double kp = 0.6, ki = 0.15, kd = 0.2; // Valores ajustados para una respuesta más lenta
PID myPID(&input, &output, &setpoint, kp, ki, kd, REVERSE);

// Intervalos de tiempo
unsigned long readInterval = 5000; // X * 1000 segundos
unsigned long pidComputeInterval = 60000UL; // 60 segundos

TemperatureFilter tempFilter;
Compressor compressor(RELAY_PIN, 4, 30); // Instancia de la clase Compressor

bool firstRun = true; // Variable para controlar el arranque inicial

inline void setupFridgeControl() {
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    myPID.SetSampleTime(pidComputeInterval); // Frecuencia de muestreo de 60 segundos para el PID
    myPID.SetMode(AUTOMATIC);
    myPID.SetOutputLimits(-1, 1); // Establecer límites de salida del PID
    Serial.begin(9600);
}

inline void loopFridgeControl() {
    unsigned long currentMillis = millis();
    //char buffer[50];
    //sprintf(buffer, "Last milliseconds value: %lu", currentMillis);
    //Serial.println(buffer);

    delay(readInterval); // Esperar el intervalo de lectura

    // Leer la temperatura
    double temperature = thermocouple.readCelsius();
    if (!isnan(temperature)) {
        tempFilter.addValue(temperature + COMPENSATION);
    }

    input = tempFilter.getFilteredValue();

    // Calcular el PID
    if (myPID.Compute()) {
        lastPIDOutput = output;
    }

    // Cambiar el estado del compresor según la salida del PID
    if (lastPIDOutput > 0.75) {
        compressor.On(currentMillis);
    } else {
        compressor.Off(currentMillis);
    }

    compressor.CheckState(currentMillis); // Comprobar el estado del compresor

    // Enviar valores por Serial Plotter
    Serial.print(input, 1);
    Serial.print(",");
    Serial.print(lastPIDOutput, 1);
    Serial.print(",");
    Serial.print(setpoint, 1);
    Serial.print(",");
    Serial.println(compressor.getState() ? 1 : 0); // Imprimir estado del compresor (1 para encendido, 0 para apagado)
}

#endif // FRIDGECONTROL_H
