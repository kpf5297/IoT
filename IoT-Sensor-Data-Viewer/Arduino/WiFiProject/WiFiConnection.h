#ifndef WIFICONNECTION_H
#define WIFICONNECTION_H

#include <WiFiNINA.h>

class WiFiConnection {
public:
    static WiFiConnection& getInstance() {
        static WiFiConnection instance;
        return instance;
    }

    void scanNetworks() {
        int n = WiFi.scanNetworks();
        Serial.println("Scan complete");
        if (n == 0) {
            Serial.println("No networks found");
        } else {
            Serial.print(n);
            Serial.println(" networks found:");
            for (int i = 0; i < n; ++i) {
                Serial.print(i + 1);
                Serial.print(": ");
                Serial.print(WiFi.SSID(i));
                Serial.print(" (");
                Serial.print(WiFi.RSSI(i));
                Serial.print(" dBm) ");
                Serial.println(getEncryptionType(WiFi.encryptionType(i)));
                delay(10);
            }
        }
    }

    int selectNetwork() {
        Serial.println("Enter the number of the network you want to connect to: ");
        while (Serial.available() == 0) {
            // wait for user input
        }
        int networkIndex = Serial.parseInt();
        // Clear the input buffer
        while (Serial.available() > 0) {
            Serial.read();
        }
        return networkIndex;
    }

    String getPassword() {
        Serial.println("Enter the password for the network: ");
        String password = "";
        while (true) {
            if (Serial.available() > 0) {
                char c = Serial.read();
                if (c == '\n') {
                    break;  // Exit loop on newline
                } else {
                    password += c;  // Append character to password
                }
            }
        }
        return password;
    }

    void connectToNetwork(int networkIndex, const String& password) {
        const char* ssid = WiFi.SSID(networkIndex - 1);
        Serial.print("Selected network: ");
        Serial.println(ssid);
        connect(ssid, password.c_str());
    }

    void connect(const char* ssid, const char* password) {
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
            Serial.println("Connecting to WiFi...");
        }
        Serial.println("Connected to WiFi");
    }

    bool isConnected() {
        return WiFi.status() == WL_CONNECTED;
    }

private:
    WiFiConnection() {}  // Private constructor
    WiFiConnection(const WiFiConnection&) = delete;
    WiFiConnection& operator=(const WiFiConnection&) = delete;

    const char* getEncryptionType(int thisType) {
        switch (thisType) {
            case ENC_TYPE_NONE:
                return "Open";
            case ENC_TYPE_WEP:
                return "WEP";
            case ENC_TYPE_TKIP:
                return "WPA";
            case ENC_TYPE_CCMP:
                return "WPA2";
            case ENC_TYPE_AUTO:
                return "Auto";
            default:
                return "Unknown";
        }
    }
};

#endif
