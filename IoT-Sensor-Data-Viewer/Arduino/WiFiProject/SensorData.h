#ifndef SENSORDATA_H
#define SENSORDATA_H

#include <Arduino.h>
#include <Arduino_LSM6DS3.h>  // Include the library for the built-in IMU sensor

class SensorData {
public:
    static SensorData& getInstance() {
        static SensorData instance;
        return instance;
    }

    void begin() {
        if (!IMU.begin()) {
            Serial.println("Failed to initialize IMU!");
            while (1);
        }
        Serial.println("IMU initialized.");
    }

    float readTemperature() {
        float temperature;
        if (IMU.temperatureAvailable()) {
            IMU.readTemperature(temperature);
        } else {
            Serial.println("Failed to read temperature from IMU!");
            return 0.0;
        }
        return temperature;
    }

    void readAcceleration(float& accelX, float& accelY, float& accelZ) {
        if (IMU.accelerationAvailable()) {
            IMU.readAcceleration(accelX, accelY, accelZ);
        } else {
            Serial.println("Failed to read acceleration from IMU!");
        }
    }

    void readGyroscope(float& gyroX, float& gyroY, float& gyroZ) {
        if (IMU.gyroscopeAvailable()) {
            IMU.readGyroscope(gyroX, gyroY, gyroZ);
        } else {
            Serial.println("Failed to read gyroscope from IMU!");
        }
    }

private:
    SensorData() {}
    ~SensorData() {}
    SensorData(const SensorData&) = delete;
    SensorData& operator=(const SensorData&) = delete;
};

#endif // SENSORDATA_H

