/*#define DEBUG_DHT*/
#ifdef DEBUG_DHT
 #define DEBUG_DHT_PRINT(x)  Serial.print (x)
 #define DEBUG_DHT_PRINTLN(x)  Serial.println (x)
#else
 #define DEBUG_DHT_PRINT(x)
 #define DEBUG_DHT_PRINTLN(x)
#endif

boolean performDHTAnalyse = false;
void setupDHT(){
  // Initialize temperature sensor
  dht.setup(DHT_PIN, DHTesp::DHT22);
  // read dht data each 20 seconde
  dhtTicker.attach(20, getDHT);
}
void onDHTTicker(){
  performDHTAnalyse = true;
}
void loopDHT(){
  if(performDHTAnalyse){
    getDHT();
    performDHTAnalyse = false;
  }
}

void getDHT(){
    DEBUG_DHT_PRINTLN("DHT getting DHT");
    DEBUG_DHT_PRINTLN("DHT getting Temperature");
    int temperature = round(dht.getTemperature()*10);
    DEBUG_DHT_PRINT(temperature);
    DEBUG_DHT_PRINTLN("°");
    DEBUG_DHT_PRINTLN("DHT getting Humidity");
    int humidity = round(dht.getHumidity()*10);
    DEBUG_DHT_PRINT(humidity);
    DEBUG_DHT_PRINTLN("%");

    setHumidity(humidity);
    setTemperature(temperature);
    
}
