#include "MockFridge.h"

MockFridge::MockFridge() 
  : temperature(15.0), compressorState(false), lastUpdateMillis(0), compressorStartMillis(0) {}

void MockFridge::updateTemperature(unsigned long currentMillis) {
    unsigned long elapsedTime = currentMillis - lastUpdateMillis;
    lastUpdateMillis = currentMillis;

    if (compressorState) {
        if (currentMillis - compressorStartMillis < 120000) { // Primeros 2 minutos
            increaseTemperature(elapsedTime);
        } else if (currentMillis - compressorStartMillis < 180000) { // Siguiente minuto
            // Mantener la temperatura constante
        } else {
            decreaseTemperature(elapsedTime);
        }
    } else {
        increaseTemperature(elapsedTime);
    }
}

double MockFridge::getTemperature() const {
    return temperature;
}

bool MockFridge::isCompressorOn() const {
    return compressorState;
}

void MockFridge::setCompressorState(bool state) {
    if (state && !compressorState) {
        compressorStartMillis = lastUpdateMillis; // Registrar el momento en que se enciende el compresor
    }
    compressorState = state;
}

void MockFridge::increaseTemperature(unsigned long elapsedTime) {
    double rate = 0.0;
    if (temperature < 20.0) {
        rate = 1.0 / 600000.0; // 1°C cada 10 minutos
    } else if (temperature < 30.0) {
        rate = 1.0 / 1800000.0; // 1°C cada 30 minutos
    }

    temperature += rate * elapsedTime;
    if (temperature > 30.0) {
        temperature = 30.0;
    }
}

void MockFridge::decreaseTemperature(unsigned long elapsedTime) {
    double rate = 1.0 / 600000.0; // 1°C cada 10 minutos
    temperature -= rate * elapsedTime;
    if (temperature < -20.0) {
        temperature = -20.0;
    }
}
