
#include <math.h>

#include <TimeLib.h>
#include <NtpClientLib.h>

#include <DHTesp.h>
#include "ThingSpeak.h"
#include <ESP8266WiFi.h>

#include <Ticker.h>

#include "secrets.h"


/*******************/
/* WIFI management */
/*******************/
WiFiClient client;
bool wifiFirstConnected = false;
static WiFiEventHandler e1, e2, e3;

/******************/
/* NTP management */
/******************/
boolean syncEventTriggered = false; // True if a time even has been triggered
NTPSyncEvent_t ntpEvent; // Last triggered event
int8_t timeZone = 1;
int8_t minutesTimeZone = 0;


/******************/
/* ECH management */
/******************/
#define ECH210BD_ADRESS 1
#define MODBUS_RX_PIN D8
#define MODBUS_TX_PIN D7
int digitalInput;
boolean compressorIn=false;
boolean boilerIn=false;
boolean pumpIn=false;
boolean warmcoolIn=false;
boolean onstandbyIn=false;
int digitalOutput;
boolean compressorOut=false;
boolean pumpOut=false;
boolean valveOut=false;
boolean boilerOut=false;
boolean alarmOut=false;

Ticker echMgrTicker;
int echMgrTickerPeriod = 10;
int echSensorsReadstatus = 0;

// Instantiate ModbusMaster object to target slave ID 1
//ESP8266ModbusMaster232 echModule(ECH210BD_ADRESS); //RX,TX D8,D7

/******************/
/* DHT management */
/******************/
#define DHT_PIN D5
DHTesp dht; // Initialize DHT sensor
Ticker dhtTicker;


/******************/
/* IOT management */
/******************/


/******************/
/* sensorData mgt */
/******************/
void storeSensorData() {
  //Serial.println("storeSensorData.");
}
void setupDS() {

}
void loopDS() {
  // store sensors data still available
  if (isDataUpdated()) {
    storeSensorData();
  }
}


#define DEBUG
#ifdef DEBUG
#define DEBUG_INIT(x) Serial.begin(x)
#define DEBUG_PRINT(x)  Serial.print (x)
#define DEBUG_PRINTLN(x)  Serial.println (x)
#define DEBUG_PRINTLN_DEC(x)  Serial.println (x,DEC)
#else
#define DEBUG_INIT(x)
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINTLN_DEC(x)
#endif

void setup() {
  delay(5000);

  DEBUG_INIT(115200);
  DEBUG_PRINTLN("application ech210 tracker starting");

  setupDS();
  setupDHT();
  setupECH();
  setupNTP();
  setupWIFI();


  // reset Wifi module reset button pushed
  WiFi.reconnect();

}

void loop() {

  loopNTP();
  loopIOT();
  loopDS();

  // reset first Wifi Connection Flag
  if (wifiFirstConnected) {
    wifiFirstConnected = false;
  }

  // mandatory for ticker management
  timeStatus();
}
