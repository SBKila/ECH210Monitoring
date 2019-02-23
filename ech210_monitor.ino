
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
#define ECH210BD_ADRESS 0x1
#define MODBUS_RX_PIN 14//13//14
#define MODBUS_TX_PIN 12//15//12
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
int echMgrTickerPeriod = 20;
int echSensorsReadstatus = 0;

// Instantiate ModbusMaster object to target slave ID 1
//ESP8266ModbusMaster232 echModule(ECH210BD_ADRESS); //RX,TX D8,D7

/******************/
/* DHT management */
/******************/
#define DHT_PIN 05
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
//  if (isDataUpdated()) {
//    storeSensorData();
//  }
}



/*******************/
/* Main management */
/*******************/
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

#define ALIVEBLINK digitalWrite(LED_BUILTIN, LOW);delay(100);digitalWrite(LED_BUILTIN, HIGH);

Ticker aliveTicker;
boolean perfomAlive = false;
void loopAlive(){
  
  if(perfomAlive){

    ALIVEBLINK
    
    if(WiFi.status()!=WL_CONNECTED){
      delay(100);
      ALIVEBLINK
    }
    
    perfomAlive=false;
  }
}

void onAliveTicker(){
  perfomAlive=true;
}
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  delay(3000);

  DEBUG_INIT(115200);
  DEBUG_PRINTLN("application ech210 tracker starting");
  
  aliveTicker.attach(10, onAliveTicker);
  
  
  setupDS();
  setupDHT();
  setupECH();
  setupNTP();
  setupWIFI();
  setupIOT();

  // reset Wifi module reset button pushed
  WiFi.reconnect();
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  
  loopAlive();
  loopNTP();

  loopDHT();
  loopECH();
  
  loopIOT();
  loopDS();
  

  // reset first Wifi Connection Flag
  if (wifiFirstConnected) {
    wifiFirstConnected = false;
  }

  // mandatory for ticker management
  timeStatus();
}
