
#include <math.h>

#include <TimeLib.h>
#include <NtpClientLib.h>

#include <DHTesp.h>
#include "ThingSpeak.h"
#include <ESP8266WiFi.h>

#include <Ticker.h>


#include <SoftwareSerial.h>
//#include <ESP8266ModbusMaster232.h>
#include "secrets.h"




// sensor data flag
boolean sensorUpdated = false;


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
sint16 sd1;
sint16 sd2;
sint16 sd3;
sint16 sd4;
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
//ESP8266ModbusMaster232 echModule(ech210BAddress); //RX,TX D8,D7

/******************/
/* DHT management */
/******************/
DHTesp dht; // Initialize DHT sensor
int dhtPin = D5; //Pin number for DHT sensor data pin
Ticker dhtTicker;
int lastTemperature = -200;
int lastHumidity = -200;

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
  if (sensorUpdated) {
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
