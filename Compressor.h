#ifndef COMPRESSOR_H
#define COMPRESSOR_H

class Compressor {
private:
    unsigned long stateChangeMillis = 0;
    bool state = false; // Estado inicial del compresor
    unsigned long maxRunTime;
    unsigned long minTimeEachStatus;
    unsigned char relayPin = 2;

public:
    // Constructor con parámetros opcionales para maxRunTime y minTimeEachStatus
    Compressor(unsigned char rp, unsigned long maxRunHrs = 4, unsigned long minTimeMins = 30)
        : relayPin(rp), 
          maxRunTime(maxRunHrs * 60UL * 60UL * 1000UL), 
          minTimeEachStatus(minTimeMins * 60UL * 1000UL) {}

    void On(unsigned long currentMillis) {
        unsigned long timeSinceStateChange = currentMillis - stateChangeMillis;
        if (!state && (timeSinceStateChange >= minTimeEachStatus || stateChangeMillis == 0)) { //Allow start on first run
            digitalWrite(relayPin, HIGH); // Encender el compresor
            digitalWrite(LED_BUILTIN, HIGH); // Encender el LED
            state = true;
            stateChangeMillis = currentMillis;
        }
    }

    void Off(unsigned long currentMillis) {
        unsigned long timeSinceStateChange = currentMillis - stateChangeMillis;
        if (state && timeSinceStateChange >= minTimeEachStatus) {
            digitalWrite(relayPin, LOW); // Apagar el compresor
            digitalWrite(LED_BUILTIN, LOW); // Apagar el LED
            state = false;
            stateChangeMillis = currentMillis;
        }
    }

    void CheckState(unsigned long currentMillis) {
        unsigned long timeSinceStateChange = currentMillis - stateChangeMillis;
        // Verificar si el compresor ha estado encendido más de 4 horas seguidas
        if (state && timeSinceStateChange >= maxRunTime) {
            Off(currentMillis);
        }
    }

    bool getState() const {
        return state;
    }
};

#endif // COMPRESSOR_H
