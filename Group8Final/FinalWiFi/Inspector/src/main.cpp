#include <Arduino.h>
#include <WiFi.h>
#include "TinyMqtt.h"
#include <string>
#include <vector>
#include "heltec.h"

#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

MqttClient mqtt_a;

const char* topicLocation = "location";
const char* topicParkingData = "parking-data";
bool flag = false;

//PIN in charge of the interrupt is the PRG pin (pin 0)
const byte interruptPin = 0;

String pub = "";
std::vector<String> availableParking; 

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    //Serial.println("test smart delay");
    while (ss.available())
      //Serial.print("ss available + encode");
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

/*
 * Callback for temp-data
 * Each publish from DUT for temp-data will be processed through this function
*/
void parkingDataCallback(const MqttClient*, const Topic& topic, const char* payload, size_t) {
  Serial.printf("--> parking-data recieved: %s\n", payload);

  //TODO: split the parking location - split back out to the user which ones are available
  bool continueFlag = false;
  char* payloadStr = (char*)payload;
  char* split;
  split = strtok(payloadStr, ":");
  while(split != NULL) {
    //Serial.printf("split: %s\n", split);
    for(int i = 0; i < availableParking.size(); i++) {
      //If the value is already in the array, do not re add it
      const char* cur = availableParking[i].c_str();
      if(!strcmp(split, cur)) {
         //Serial.println("Repeat reading...");
         continueFlag = true;
         break;
      }
    }
    if(continueFlag) {
      split = strtok(NULL,":");
      continueFlag = false;
      continue;
    } else {
      availableParking.push_back(split);
      //Serial.println("test");
      split = strtok(NULL,":");
    }
  }
  //availableParking has the x + y coordinates of an available parking spot
  Serial.println("Parking available at:");
  Serial.println("# | Lat | Long");
  for(int i = 0; i < availableParking.size(); i++) {
    Serial.printf("%d: %s\n", i, availableParking[i]);
  }
  Serial.println("------------------------------------");
  availableParking.clear();
}

/*
 * Button interrupt handler
 * When button is pressed (interrupt), this function will be ran to handle it.
*/
void controlMsgIrq() {
  smartDelay(1);
  Serial.println("publishing current location");
  //double lat = gps.location.lat();
  //double lng = gps.location.lng();
  double lat = 40.00;
  double lng = 40.00;
  char temp[20];
  sprintf(temp, "%.10f", lat);
  pub = temp;
  sprintf(temp, "%.10f", lng);
  pub = pub + " " + temp;

  //RECONMENDED NOT TO PUBLISH IN IRQ,
  //SET FLAG AND PUBLISH IN LOOP
  flag = true;
  Serial.println("publishing this:");
  Serial.println(pub);
}

void setup() {
  Serial.begin(115200);
  ss.begin(GPSBaud);
  /*
   * Connecting to AP station.
  */
  WiFi.mode(WIFI_STA);
  WiFi.begin("Group8", "passphrase");
  while(WiFi.status() != WL_CONNECTED) { Serial.println("-"); delay(500); }

  Serial.printf("Connected to Group8 -- Inspector\n");

  //Starting the Heltec Board
  Heltec.begin(true, false, true);
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);

  //button interrupt
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), controlMsgIrq, RISING);
  
  //Connecting to the broker
  mqtt_a.connect("192.168.4.1", 1883, 65535);
  if(mqtt_a.connected()) 
    Serial.printf("Connected to Broker -- Inspector\n");

  mqtt_a.setCallback(parkingDataCallback);
  mqtt_a.subscribe(topicParkingData);
}

void loop() {
  mqtt_a.loop();
  smartDelay(1000);
  /*
   * Loop to average the temperture data
  */
  static const int interval = 60000;
  static uint32_t timer = millis() + interval;

  if(millis() > timer) {
    /*
    //Averaging all the readings from the past minute (stored in a vector)
    //Print statements for debugging
    double tempSum = 0;
    for(int i = 0; i < parkingData.size(); i++) {
      Serial.print((String)parkingData.at(i) + ", ");
      tempSum += parkingData.at(i);
    }
    Serial.println(tempSum);
    Serial.println(parkingData.size());

    double tempAvg = tempSum/parkingData.size();
    Serial.printf("1 min. has passed. Printing average temp-data: %f\n", tempAvg);

    //Displaying the average in the past minute on Heltec OLED
    String boardDisplay = "Average temp (past 1 min.) " + (String)tempAvg;
    Heltec.display->clear();
    Heltec.display->drawStringMaxWidth(0, 0, 128, boardDisplay);
    Heltec.display->display();
    //clearing array for next minute
    parkingData.clear();
    timer += interval;
    */
  }


  if(flag) {
    flag = false;
    //Serial.println("Actually publishing now");
    mqtt_a.publish(topicLocation, pub);
  }
}
