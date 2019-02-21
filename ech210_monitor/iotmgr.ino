#define DEBUG_IOT
#ifdef DEBUG_IOT
 #define DEBUG_IOT_PRINT(x)  Serial.print (x)
 #define DEBUG_IOT_PRINTLN(x)  Serial.println (x)
#else
 #define DEBUG_IOT_PRINT(x)
 #define DEBUG_IOT_PRINTLN(x)
#endif

void setupIOT(){
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loopIOT(){
  if(WiFi.status()==WL_CONNECTED){
    if(isDataUpdated()){
      sendToThingsSpeak2();
    }
  } 
}

void sendToThingsSpeak2(){
  DEBUG_IOT_PRINT("getting Measure Signal Strength");
  long rssi = WiFi.RSSI();
  DEBUG_IOT_PRINTLN(rssi);
  ThingSpeak.setField(1, String(rssi));
  DEBUG_IOT_PRINTLN("getting Humidity");
  ThingSpeak.setField(2, String(getHumidity(), 1));
  DEBUG_IOT_PRINTLN("getting Temperature");
  ThingSpeak.setField(3, String(getTemperature(), 1));
  DEBUG_IOT_PRINTLN("getting SD1");
  ThingSpeak.setField(4, String(getSD1()));
  DEBUG_IOT_PRINTLN("getting SD2");
  ThingSpeak.setField(4, String(getSD2()));
  DEBUG_IOT_PRINTLN("getting SD3");
  ThingSpeak.setField(4, String(getSD3()));
  DEBUG_IOT_PRINTLN("getting SD4");
  ThingSpeak.setField(4, String(getSD4()));
  
  // write to the ThingSpeak channel
   DEBUG_IOT_PRINTLN("IOT Channel updating.....");
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    DEBUG_IOT_PRINTLN("IOT Channel update successful.");
    resetDataUpdated();
  }
  else{
    DEBUG_IOT_PRINTLN("IOT Problem updating channel. HTTP error code " + String(x));
  }

}
