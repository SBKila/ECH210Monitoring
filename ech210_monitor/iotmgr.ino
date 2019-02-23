/*#define DEBUG_IOT*/
#ifdef DEBUG_IOT
#define DEBUG_IOT_PRINT(x)  Serial.print (x)
#define DEBUG_IOT_PRINTLN(x)  Serial.println (x)
#define DEBUG_IOT_PRINT_NUM(x,y)  Serial.print (x,y)
#define DEBUG_IOT_PRINTLN_NUM(x,y)  Serial.println (x,y)
#else
#define DEBUG_IOT_PRINT(x)
#define DEBUG_IOT_PRINTLN(x)
#define DEBUG_IOT_PRINT_NUM(x,y)
#define DEBUG_IOT_PRINTLN_NUM(x,y)
#endif

void setupIOT(){
  DEBUG_IOT_PRINTLN("setupIOT");
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loopIOT(){
  if(WiFi.status()==WL_CONNECTED){
    if(isDataUpdated()){
      sendToThingsSpeak2();
    }
  } 
}
void setField(unsigned int field, long value){
  if(value!=-600){
    ThingSpeak.setField(field, value);
  }
}
void sendToThingsSpeak2() {
  DEBUG_IOT_PRINT("getting Measure Signal Strength");
  long rssi = WiFi.RSSI();
  DEBUG_IOT_PRINTLN(rssi);
  ThingSpeak.setField(1, String(rssi));
  DEBUG_IOT_PRINTLN("getting Humidity");
  ThingSpeak.setField(2,getHumidity());
  DEBUG_IOT_PRINTLN("getting Temperature");
  ThingSpeak.setField(3, getTemperature());

  DEBUG_IOT_PRINTLN("getting SD1");
  setField(4, getSD1());
  DEBUG_IOT_PRINTLN("getting SD2");
  setField(5, getSD2());
  DEBUG_IOT_PRINTLN("getting SD3");
  setField(6, getSD3());
  DEBUG_IOT_PRINTLN("getting SD4");
  setField(7, getSD4());
  DEBUG_IOT_PRINTLN("getting DO");
  setField(8, getDigitalOutput());
  
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
