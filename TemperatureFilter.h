#ifndef TEMPERATUREFILTER_H
#define TEMPERATUREFILTER_H

class TemperatureFilter {
public:
    TemperatureFilter(unsigned int size = 10) : size(size), index(0), count(0) {
        values = new double[size];
    }

    ~TemperatureFilter() {
        delete[] values;
    }

    void addValue(double value) {
        values[index] = value;
        index = (index + 1) % size;
        if (count < size) {
            count++;
        }
    }

    double getFilteredValue() {
        if (count == 0) return 0; // Si no hay valores, devolver 0
        if (count <= 2) return values[0]; // Si hay menos de 3 valores, devolver el primero

        double sum = 0;
        double minVal = values[0];
        double maxVal = values[0];
        for (unsigned int i = 0; i < count; i++) {
            sum += values[i];
            if (values[i] < minVal) minVal = values[i];
            if (values[i] > maxVal) maxVal = values[i];
        }

        // Excluir el valor mínimo y máximo
        sum -= minVal;
        sum -= maxVal;
        return sum / (count - 2);
    }

private:
    double* values;
    unsigned int size;
    unsigned int index;
    unsigned int count;
};

#endif // TEMPERATUREFILTER_H
