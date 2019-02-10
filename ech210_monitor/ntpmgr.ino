/*#define DEBUG_NTP*/
#ifdef DEBUG_NTP
 #define DEBUG_NTPCLIENT
 #define DEBUG_NTP_PRINT(x)  Serial.print (x)
 #define DEBUG_NTP_PRINTLN(x)  Serial.println (x)
 #define DEBUG_NTP_PRINTLN_DEC(x)  Serial.println (x,DEC)
#else
 #define DEBUG_NTP_PRINT(x)
 #define DEBUG_NTP_PRINTLN(x)
 #define DEBUG_NTP_PRINTLN_DEC(x)
#endif


void processSyncEvent (NTPSyncEvent_t ntpEvent) {
  if (ntpEvent) {
    DEBUG_NTP_PRINT("NTP Time Sync error: ");
    if (ntpEvent == noResponse)
      DEBUG_NTP_PRINTLN("server not reachable");
    else if (ntpEvent == invalidAddress)
      DEBUG_NTP_PRINT(" invalid server address");
  } else {
    DEBUG_NTP_PRINT("NTP Got NTP time: ");
    DEBUG_NTP_PRINTLN(NTP.getTimeDateString (NTP.getLastNTPSync ()));
  }
}

void setupNTP(){
  NTP.onNTPSyncEvent ([](NTPSyncEvent_t event) {
    ntpEvent = event;
    syncEventTriggered = true;
  });
  
}
void loopNTP(){
  
  // init NTP service at first connection
  if (wifiFirstConnected) {
    NTP.begin ("pool.ntp.org", timeZone, true, minutesTimeZone);
    // get NTP each hours
    NTP.setInterval (3600);
  } else {
    // treat synchronization event 
    if (syncEventTriggered) {
      processSyncEvent (ntpEvent);
      syncEventTriggered = false;
    }
  }
}
