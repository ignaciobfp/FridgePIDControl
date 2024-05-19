#ifndef MOCKFRIDGE_H
#define MOCKFRIDGE_H

class MockFridge {
public:
    MockFridge();
    void updateTemperature(unsigned long currentMillis);
    double getTemperature() const;
    bool isCompressorOn() const;
    void setCompressorState(bool state);

private:
    double temperature;
    bool compressorState;
    unsigned long lastUpdateMillis;
    unsigned long compressorStartMillis;
    void increaseTemperature(unsigned long elapsedTime);
    void decreaseTemperature(unsigned long elapsedTime);
};

#endif // MOCKFRIDGE_H
