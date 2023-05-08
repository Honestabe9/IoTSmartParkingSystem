#include <Arduino.h>
#include <WiFi.h>
#include "driver/temp_sensor.h"   
#include "TinyMqtt.h"

typedef struct {
  float coordNWx;
  float coordNWy;
  float coordNEx;
  float coordNEy;
  float coordSWx;
  float coordSWy;
  float coordSEx;
  float coordSEy;
  bool isTaken = false;
} parkingSpot;

MqttClient mqtt_a;

const char* topicParkingData = "parking-data";
const char* topicLocation = "location";
int interval = 5000;
uint32_t timer = millis() + interval;
std::vector<parkingSpot> parkingArr;
String out = "";
std::vector<float> location; 

/*
* Callback function for location data
* If the coordinates sent are within the range of a parking spot, switch the isTaken value
*/
void msgCallback(const MqttClient*, const Topic& topic, const char* payload, size_t)
{
 Serial.printf("--> location received: %s \n", payload);
 char* payloadStr = (char*)payload;
 //strtok() splits a string by deliminator -- we want to split by a space " "
 char* split;
 split = strtok(payloadStr, " ");
 while(split != NULL) {
    location.push_back(atof(split));
    split = strtok(NULL," ");
 }

  bool changed = false;
 //At this moment, location[0] holds latitude, location[1] holds longitude
  for(int i = 0; i < parkingArr.size(); i++) {
    if( (location[0] >=  parkingArr[i].coordSEy && parkingArr[i].coordSWy) &&
        (location[0] <= parkingArr[i].coordNEy && parkingArr[i].coordNWy) &&
        (location[1] >= parkingArr[i].coordNWx && parkingArr[i].coordSWx) &&
        (location[1] <= parkingArr[i].coordNEx && parkingArr[i].coordSEx)) {
          //switch the parking status
          Serial.println("Changing the parking status");
          parkingArr[i].isTaken = !(parkingArr[i].isTaken);
          changed = true;
        }
  }
  if(!changed) {
      Serial.println("Not in valid parking spot");
     }
  location.clear();
}

void setup() {
  Serial.begin(115200);

  /*
  * Connect to AP station.
  */
  WiFi.mode(WIFI_STA);
  WiFi.begin("Group8", "passphrase");
  while(WiFi.status() != WL_CONNECTED) { Serial.println("-"); delay(500);}

  Serial.println("Connected to Group8 IP Addr -- DUT\n");
 
  //Connecting to broker
  mqtt_a.connect("192.168.4.1", 1883, 65535);
  if(mqtt_a.connected()) {
    Serial.printf("Connected to Broker -- DUT\n");
  }

  //Subscribing to control-msgs
  mqtt_a.setCallback(msgCallback);
  mqtt_a.subscribe(topicLocation);

  /* Setting parking spot parameters*/
  parkingSpot testSpot;
  testSpot.coordNWx = 38;
  testSpot.coordNWy = -78;
  testSpot.coordNEx = 39;
  testSpot.coordNEy = -78;
  testSpot.coordSWx = 38;
  testSpot.coordSWy = -79;
  testSpot.coordSEx = 39;
  testSpot.coordSEy = -79;
  parkingArr.push_back(testSpot);

  parkingSpot testSpot2;
  testSpot2.coordNWx = 40;
  testSpot2.coordNWy = 40;
  testSpot2.coordNEx = 40;
  testSpot2.coordNEy = 40;
  testSpot2.coordSWx = 40;
  testSpot2.coordSWy = 40;
  testSpot2.coordSEx = 40;
  testSpot2.coordSEy = 40;
  parkingArr.push_back(testSpot2);

  parkingSpot testSpot3;
  testSpot3.coordNWx = 20;
  testSpot3.coordNWy = 20;
  testSpot3.coordNEx = 20;
  testSpot3.coordNEy = 20;
  testSpot3.coordSWx = 20;
  testSpot3.coordSWy = 20;
  testSpot3.coordSEx = 20;
  testSpot3.coordSEy = 20;
  testSpot3.isTaken = true;
  parkingArr.push_back(testSpot3);

  parkingSpot spot1;
  spot1.coordNWx = 38.026446;
  spot1.coordNWy = -78.507346;
  spot1.coordNEx = 38.026337;
  spot1.coordNEy = -78.507376;
  spot1.coordSWx = 38.026220;
  spot1.coordSWy = -78.507375;
  spot1.coordSEx = 38.026316;
  spot1.coordSEy = -78.507425;
  parkingArr.push_back(spot1);
}

void loop() {
  mqtt_a.loop();
  /*
  * Publish the temp data (based on interval)
  */
  if(millis() > timer)
  {
    Serial.println("parking-data is publishing");
    timer += interval;
    for(int i = 0; i < parkingArr.size(); i++) {
      if(!parkingArr[i].isTaken) {
        String Xmidpoint = (String)((parkingArr[i].coordNWx + parkingArr[i].coordNEx + parkingArr[i].coordSEx + parkingArr[i].coordSWx)/4);
        String Ymidpoint = (String)((parkingArr[i].coordNWy + parkingArr[i].coordNEy + parkingArr[i].coordSEy + parkingArr[i].coordSWy)/4);
        out = out + Xmidpoint + " " + Ymidpoint + ":";
        //Serial.println(out);
      }
    }
    Serial.println("publishing this :");
    Serial.println(out);
    mqtt_a.publish(topicParkingData, out);
    out = "";
  }
}
