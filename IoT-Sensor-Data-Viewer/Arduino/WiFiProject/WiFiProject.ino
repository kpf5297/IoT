#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include "WiFiConnection.h"
#include "SensorData.h"

const char* serverAddress = "192.168.1.152";  // Replace with your Raspberry Pi's IP address
const int port = 5000;

WiFiClient wifiClient;
HttpClient client = HttpClient(wifiClient, serverAddress, port);

void setup() {
    Serial.begin(115200);

    // Initialize the sensor
    SensorData::getInstance().begin();

    // Connect to WiFi
    Serial.println("Scanning for available networks...");
    WiFiConnection::getInstance().scanNetworks();

    // Select network
    int networkIndex = WiFiConnection::getInstance().selectNetwork();

    // Get password
    String password = WiFiConnection::getInstance().getPassword();

    // Connect to the selected network
    WiFiConnection::getInstance().connectToNetwork(networkIndex, password);

    if (WiFiConnection::getInstance().isConnected()) {
        Serial.println("WiFi connected");
    }
}

void loop() {
    if (WiFiConnection::getInstance().isConnected()) {
        Serial.println("Sending data to server...");

        // Read data from the sensor
        float temperature = SensorData::getInstance().readTemperature();

        // Dummy data for other sensors (or real data if available)
        float humidity = 0.0;  // Assuming no humidity sensor, set to 0.0
        int mic = 100;         // Dummy microphone data
        float accelX, accelY, accelZ;
        SensorData::getInstance().readAcceleration(accelX, accelY, accelZ);
        float gyroX, gyroY, gyroZ;
        SensorData::getInstance().readGyroscope(gyroX, gyroY, gyroZ);

        // Create JSON payload
        String payload = "{\"temperature\":";
        payload += temperature;
        payload += ",\"humidity\":";
        payload += humidity;
        payload += ",\"mic\":";
        payload += mic;
        payload += ",\"accelX\":";
        payload += accelX;
        payload += ",\"accelY\":";
        payload += accelY;
        payload += ",\"accelZ\":";
        payload += accelZ;
        payload += ",\"gyroX\":";
        payload += gyroX;
        payload += ",\"gyroY\":";
        payload += gyroY;
        payload += ",\"gyroZ\":";
        payload += gyroZ;
        payload += "}";

        client.beginRequest();
        client.post("/add_data");
        client.sendHeader("Content-Type", "application/json");
        client.sendHeader("Content-Length", payload.length());
        client.beginBody();
        client.print(payload);
        client.endRequest();

        int statusCode = client.responseStatusCode();
        String response = client.responseBody();

        Serial.print("Status code: ");
        Serial.println(statusCode);
        Serial.print("Response: ");
        Serial.println(response);

        delay(500);  // Wait 500ms before sending next data
    }
}
