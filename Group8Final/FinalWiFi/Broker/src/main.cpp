#include <Arduino.h>
#include "TinyMQTT.h"
#include <Arduino.h>
#include <WiFi.h> // arduino-esp32 wifi api
#include <esp_wifi.h> // esp-idf wifi api
#define PORT 1883

MqttBroker broker(PORT);

uint32_t temp = millis() + 1000;

void setup()
{
    Serial.begin(115200);
    delay(10);

    Serial.println("Started!");
    Serial.print("MAC Address is: ");
    Serial.println(WiFi.macAddress());

    Serial.println("Setting up Access Point...");
    if (!WiFi.softAP("Group8", "passphrase")) {
        Serial.println("Soft AP creation failed.");
        while(1);
    }
    broker.begin();
    Serial.println("AP Started!");
    IPAddress myIP = WiFi.softAPIP();
    Serial.println(myIP);
}

void get_client_details() {
    wifi_sta_list_t clients;
    esp_wifi_ap_get_sta_list(&clients);
    Serial.printf("Connected clients: %d\n", clients.num);

    for (size_t i=0; i<clients.num; i++) {
        wifi_sta_info_t* info = &(clients.sta[i]);
        const char* phys = "Unknown";
        if (info->phy_11b && info->phy_11g && info->phy_11n) {
            phys = "802.11b/g/n";
        }
        Serial.printf("%d\tRSSI=%d\tPHY=%s\n", i,
            info->rssi, phys);
    }
}

void loop()
{
    broker.loop();
    if(millis() > temp){
        get_client_details();
        temp += 1000;
    }
}
