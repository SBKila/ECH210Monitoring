//#define DEBUG_WIFIMGR
#ifdef DEBUG_WIFIMGR
 #define DEBUG_WIFI_PRINT(x)  Serial.print (x)
 #define DEBUG_WIFI_PRINTLN(x)  Serial.println (x)
 #define DEBUG_WIFI_PRINTLN_DEC(x)  Serial.println (x,DEC)
#else
 #define DEBUG_WIFI_PRINT(x)
 #define DEBUG_WIFI_PRINTLN(x)
 #define DEBUG_WIFI_PRINTLN_DEC(x)
#endif

void setupWIFI(){
  DEBUG_WIFI_PRINTLN("setupWIFI");
  e1 = WiFi.onStationModeGotIP (onSTAGotIP);// As soon WiFi is connected, start NTP Client
  e2 = WiFi.onStationModeDisconnected (onSTADisconnected);
  e3 = WiFi.onStationModeConnected (onSTAConnected);

  WiFi.mode (WIFI_STA);
  WiFi.begin(ssid, password);
}

void onSTAConnected (WiFiEventStationModeConnected ipInfo) {
  DEBUG_WIFI_PRINT("WIFI Connected to: ");
  DEBUG_WIFI_PRINTLN(ipInfo.ssid);
}

// Start NTP only after IP network is connected
void onSTAGotIP (WiFiEventStationModeGotIP ipInfo) {
  DEBUG_WIFI_PRINT("WIFI Got IP: ");
  DEBUG_WIFI_PRINTLN(ipInfo.ip.toString());
  DEBUG_WIFI_PRINT("WIFI Connection status: ");
  DEBUG_WIFI_PRINTLN(WiFi.status () == WL_CONNECTED ? "connected" : "not connected");
  wifiFirstConnected = WiFi.status () == WL_CONNECTED;
}

// Manage network disconnection
void onSTADisconnected (WiFiEventStationModeDisconnected event_info) {
  DEBUG_WIFI_PRINT("WIFI Disconnected from ");
  DEBUG_WIFI_PRINT(event_info.ssid);
  DEBUG_WIFI_PRINT(" because of : ");
  DEBUG_WIFI_PRINTLN_DEC(event_info.reason);
  NTP.stop(); // NTP sync can be disabled to avoid sync errors
}
