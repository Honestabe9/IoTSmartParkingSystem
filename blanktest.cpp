#include "LoRaWan_APP.h"
#include <Arduino.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

uint8_t devEui[] = {};  
bool overTheAirActivation = false;
uint8_t appEui[] = {};  // you should set whatever your TTN generates. TTN calls this the joinEUI, they are the same thing. 
uint8_t appKey[] = {};  // you should set whatever your TTN generates 

//These are only used for ABP, for OTAA, these values are generated on the Nwk Server, you should not have to change these values
uint8_t nwkSKey[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t appSKey[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint32_t devAddr =  (uint32_t)0x00000000;  

/*LoraWan channelsmask*/
uint16_t userChannelsMask[6]={ 0xFF00,0x0000,0x0000,0x0000,0x0000,0x0000 };

/*LoraWan region, select in arduino IDE tools*/
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;  // we define this as a user flag in the .ini file. 

/*LoraWan Class, Class A and Class C are supported*/
DeviceClass_t  loraWanClass = CLASS_A;

/*the application data transmission duty cycle.  value in [ms].*/
uint32_t appTxDutyCycle = 15000;

/*ADR enable*/
bool loraWanAdr = true;

// uint32_t license[4] = {};

/* Indicates if the node is sending confirmed or unconfirmed messages */
bool isTxConfirmed = true;

/* Application port */
uint8_t appPort = 1;
/*!
* Number of trials to transmit the frame, if the LoRaMAC layer did not
* receive an acknowledgment. The MAC performs a datarate adaptation,
* according to the LoRaWAN Specification V1.0.2, chapter 18.4, according
* to the following table:
*
* Transmission nb | Data Rate
* ----------------|-----------
* 1 (first)       | DR
* 2               | DR
* 3               | max(DR-1,0)
* 4               | max(DR-1,0)
* 5               | max(DR-2,0)
* 6               | max(DR-2,0)
* 7               | max(DR-3,0)
* 8               | max(DR-3,0)
*
* Note, that if NbTrials is set to 1 or 2, the MAC will not decrease
* the datarate, in case the LoRaMAC layer did not receive an acknowledgment
*/
uint8_t confirmedNbTrials = 8;

static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

const byte button = 0;

// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

/* Prepares the payload of the frame */
static void prepareTxFrame( uint8_t port )
{
	/*appData size is LORAWAN_APP_DATA_MAX_SIZE which is defined in "commissioning.h".
	*appDataSize max value is LORAWAN_APP_DATA_MAX_SIZE.
	*if enabled AT, don't modify LORAWAN_APP_DATA_MAX_SIZE, it may cause system hanging or failure.
	*if disabled AT, LORAWAN_APP_DATA_MAX_SIZE can be modified, the max value is reference to lorawan region and SF.
	*for example, if use REGION_CN470, 
	*the max value for different DR can be found in MaxPayloadOfDatarateCN470 refer to DataratesCN470 and BandwidthsCN470 in "RegionCN470.h".
	*/
    // This data can be changed, just make sure to change the datasize as well. 
    appDataSize = 26;
	//first byte for group 8 -- needed for decoder
    appData[0] = 0x08;
	//second byte for whether the parking spot is occupied or not -- 1 for occupied, 0 for unoccupied
    appData[1] = 0x00;
	
	// bytes 2-13 for latitude
	long long lat = gps.location.lat();
	int temp = lat%100;
    appData[2] = temp;
	lat = gps.location.lat() * 100;
	temp = lat%100;
    appData[3] = temp;
	lat = gps.location.lat() * 10000;
	temp = lat%100;
	appData[4] = temp;
	lat = gps.location.lat() * 1000000;
	temp = lat%100;
	appData[5] = temp;
	lat = gps.location.lat() * 100000000;
	temp = lat%100;
	appData[6] = temp;
	lat = gps.location.lat() * 10000000000;
	temp = lat%100;
	appData[7] = temp;

	//bytes 8-13 are for longitude
	long long lng = gps.location.lat();
	int temp = lng%100;
	appData[8] = temp;
	lat = gps.location.lat() * 100;
	temp = lat%100;
	appData[9] = temp;
	lat = gps.location.lat() * 10000;
	temp = lat%100;
	appData[10] = temp;
	lat = gps.location.lat() * 1000000;
	temp = lat%100;
	appData[11] = temp;
	lat = gps.location.lat() * 100000000;
	temp = lat%100;
	appData[12] = temp;
	lat = gps.location.lat() * 10000000000;
	temp = lat%100;
	appData[13] = temp;

	
	Serial.println("Latitude: ");
	Serial.println(gps.location.lat(), 10);
	Serial.println("Longitude: ");
  	Serial.println(gps.location.lng(), 10);
	
	//long lat = gps.location.lat() * 10000000000;
	/*
	long long lat = gps.location.lat() * 100;
	Serial.println(lat);
	Serial.println(lat%100);

	lat = gps.location.lat() * 10000;
	Serial.println(lat);
	Serial.println(lat%100);

	lat = gps.location.lat() * 1000000;
	Serial.println(lat);
	Serial.println(lat%100);

	lat = gps.location.lat() * 100000000;
	Serial.println(lat);
	Serial.println(lat%100);

	lat = gps.location.lat() * 10000000000;
	Serial.println(lat);
	Serial.println(lat%100);
	*/
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

/*
 * Button interrupt handler
 * When button is pressed (interrupt), this function will be ran to handle it.
*/
void buttonIRQ() {
  deviceState = DEVICE_STATE_SEND;
}

RTC_DATA_ATTR bool firstrun = true;

void setup() {
  Serial.begin(9600);
  ss.begin(GPSBaud);
  smartDelay(0);

  //button interrupt
  pinMode(button, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(button), buttonIRQ, RISING);

  Mcu.begin();
  if(firstrun)
  {
    LoRaWAN.displayMcuInit();
    firstrun = false;
  }
	deviceState = DEVICE_STATE_INIT;
}

void loop()
{
	switch( deviceState )
	{
		case DEVICE_STATE_INIT:
		{
#if(LORAWAN_DEVEUI_AUTO)
			LoRaWAN.generateDeveuiByChipID();
#endif
			LoRaWAN.init(loraWanClass,loraWanRegion);
			break;
		}
		case DEVICE_STATE_JOIN:
		{
      LoRaWAN.displayJoining();
			LoRaWAN.join();
			if (deviceState == DEVICE_STATE_SEND){
			 	LoRaWAN.displayJoined();
			}
			break;
		}
		case DEVICE_STATE_SEND:
		{
      		LoRaWAN.displaySending();
			prepareTxFrame( appPort );
			LoRaWAN.send();
			deviceState = DEVICE_STATE_SLEEP;
			break;
		}
		case DEVICE_STATE_CYCLE:
		{
			// Schedule next packet transmission
			txDutyCycleTime = appTxDutyCycle + randr( 0, APP_TX_DUTYCYCLE_RND );
			LoRaWAN.cycle(txDutyCycleTime);
			deviceState = DEVICE_STATE_SLEEP;
			break;
		}
		case DEVICE_STATE_SLEEP:
		{
      		LoRaWAN.displayAck();
			LoRaWAN.sleep(loraWanClass);
			break;
		}
		default:
		{
			deviceState = DEVICE_STATE_INIT;
			break;
		}
	}
	smartDelay(1000);
}
