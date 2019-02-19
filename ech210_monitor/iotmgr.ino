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
    sendToThingsSpeak2();
    if(sensorUpdated){
      sendToThingsSpeak2();
      /**************************/
      sensorUpdated=false;
      /**************************/
    }
  } 
}

void sendToThingsSpeak2(){
  //Serial.println("getting Measure Signal Strength");
  long rssi = WiFi.RSSI();
  ThingSpeak.setField(1, String(rssi));
  ThingSpeak.setField(2, String(lastHumidity, 1));
  ThingSpeak.setField(3, String(lastTemperature, 1));
  ThingSpeak.setField(4, String(sd1));
  ThingSpeak.setField(4, String(sd2));
  ThingSpeak.setField(4, String(sd3));
  ThingSpeak.setField(4, String(sd4));
  
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    DEBUG_IOT_PRINTLN("IOT Channel update successful.");
  }
  else{
    DEBUG_IOT_PRINTLN("IOT Problem updating channel. HTTP error code " + String(x));
  }

}
