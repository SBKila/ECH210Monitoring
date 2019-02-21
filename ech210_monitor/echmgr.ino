#define DEBUG_ECH
#ifdef DEBUG_ECH
 #define DEBUG_ECH_PRINT(x)  Serial.print (x)
 #define DEBUG_ECH_PRINTLN(x)  Serial.println (x)
 #define DEBUG_ECH_PRINT_DEC(x)  Serial.print (x,DEC)
 #define DEBUG_ECH_PRINTLN_DEC(x)  Serial.println (x,DEC)
#else
 #define DEBUG_ECH_PRINT(x)
 #define DEBUG_ECH_PRINTLN(x)
 #define DEBUG_ECH_PRINT_DEC(x)
 #define DEBUG_ECH_PRINTLN_DEC(x)
#endif

#define TP_WATERIN 1135
#define TP_WATEROUT 1137
#define TP_CONDENSOR 1139
#define TP_OUTDOOR 1147

#define ADDR_DIGITAL_INPUT 1124
#define MASK_DI_COMPRESSOR   0x40
#define MASK_DI_BOILER       0x60
#define MASK_DI_PUMP         0x08
#define MASK_DI_WARMCOOL     0x10
#define MASK_DI_ONOFFSTANDBY 0x20


#define ADDR_DIGITAL_OUTPUT    1188
#define MASK_DO_COMPRESSOR   0x01
#define MASK_DO_PUMP         0x02
#define MASK_DO_REVERSAL     0x04
#define MASK_DO_BOILER       0x08
#define MASK_DO_ALARM        0x10
#define MASK_DO_TBD          0x20



void setupECH() {
  DEBUG_ECH_PRINTLN("setup_echmgr");

  ESP8266ModbusMaster232_init(ECH210BD_ADRESS);
  
  // Initialize Modbus communication baud rate
  ESP8266ModbusMaster232_begin(9600);

  // Initalize echModule sensor value reading
  echMgrTicker.attach(echMgrTickerPeriod, read_EchSensors);
}

int Mdelay = 200;
// Read ECH Module sensor values
void read_EchSensors() {
  
  sint16 value;

  DEBUG_ECH_PRINTLN("");
  DEBUG_ECH_PRINTLN("");
  DEBUG_ECH_PRINTLN("");

  
  DEBUG_ECH_PRINT("ECH Reading Analog Input ");
  int result = ESP8266ModbusMaster232_readHoldingRegisters(TP_WATERIN, 1);
  if(result!=0){
    DEBUG_ECH_PRINTLN(" ECH Error reading TP_WATERIN");
    return;
  }
  value=ESP8266ModbusMaster232_getResponseBuffer(0);
  setSD1(value);
  ESP8266ModbusMaster232_clearResponseBuffer();
  delay(Mdelay);
  DEBUG_ECH_PRINT("sd1:");
  DEBUG_ECH_PRINT(value);

  result =  ESP8266ModbusMaster232_readHoldingRegisters(TP_WATEROUT, 1);
  if(result!=0){
    DEBUG_ECH_PRINTLN(" ECH Error reading TP_WATEROUT");
    return;
  }
  value=ESP8266ModbusMaster232_getResponseBuffer(0);
  setSD2(value);
  ESP8266ModbusMaster232_clearResponseBuffer();
  delay(Mdelay);
  DEBUG_ECH_PRINT(" sd2:");
  DEBUG_ECH_PRINT(value);

  result =  ESP8266ModbusMaster232_readHoldingRegisters(TP_CONDENSOR, 1);
  if(result!=0){
    DEBUG_ECH_PRINTLN(" ECH Error reading TP_CONDENSOR");
    return;
  }
  value=ESP8266ModbusMaster232_getResponseBuffer(0);
  setSD3(value);
  ESP8266ModbusMaster232_clearResponseBuffer();
  delay(Mdelay); 
  DEBUG_ECH_PRINT(" sd3:");
  DEBUG_ECH_PRINT(value);

  result =  ESP8266ModbusMaster232_readHoldingRegisters(TP_OUTDOOR, 1);
  if(result!=0){
    DEBUG_ECH_PRINTLN(" ECH Error reading TP_OUTDOOR");
    return;
  }
  value=ESP8266ModbusMaster232_getResponseBuffer(0);
  setSD4(value);
  ESP8266ModbusMaster232_clearResponseBuffer();
  delay(Mdelay); 
  DEBUG_ECH_PRINT(" sd4:");
  DEBUG_ECH_PRINT(value);
  

  DEBUG_ECH_PRINTLN("ECH Reading DigitalInput ");
  result =  ESP8266ModbusMaster232_readHoldingRegisters(ADDR_DIGITAL_INPUT, 1);
  if(result!=0){
    DEBUG_ECH_PRINTLN("ECH Error reading ECHSensors");
    return;
  }
  digitalInput=ESP8266ModbusMaster232_getResponseBuffer(0);
  ESP8266ModbusMaster232_clearResponseBuffer();
  delay(Mdelay);

  compressorIn=(0 != (digitalInput & MASK_DI_COMPRESSOR));
  boilerIn=(0 != (digitalInput & MASK_DI_BOILER));
  pumpIn=(0 != (digitalInput & MASK_DI_PUMP));
  warmcoolIn=(0 != (digitalInput & MASK_DI_WARMCOOL));
  onstandbyIn=(0 != (digitalInput & MASK_DO_REVERSAL));
  DEBUG_ECH_PRINT("compressorIn:");
  DEBUG_ECH_PRINT(compressorIn);
  DEBUG_ECH_PRINT(" boilerIn:");
  DEBUG_ECH_PRINT(boilerIn);
  DEBUG_ECH_PRINT(" warmcoolIn:");
  DEBUG_ECH_PRINT(warmcoolIn);
  DEBUG_ECH_PRINT(" onstandbyIn:");
  DEBUG_ECH_PRINTLN(onstandbyIn);
  
  DEBUG_ECH_PRINTLN("ECH Reading DigitalOutput ");
  result =  ESP8266ModbusMaster232_readHoldingRegisters(ADDR_DIGITAL_OUTPUT, 1);
  if(result!=0){
    DEBUG_ECH_PRINTLN("ECH Error reading ECHSensors");
    return;
  }
  digitalOutput=ESP8266ModbusMaster232_getResponseBuffer(0);
  ESP8266ModbusMaster232_clearResponseBuffer();
  delay(Mdelay);
  compressorOut=(0 != (digitalOutput & MASK_DO_COMPRESSOR));
  pumpOut=(0 != (digitalOutput & MASK_DO_PUMP));
  valveOut=(0 != (digitalOutput & MASK_DO_REVERSAL));
  boilerOut=(0 != (digitalOutput & MASK_DO_COMPRESSOR));
  alarmOut=(0 != (digitalOutput & MASK_DO_ALARM));
  DEBUG_ECH_PRINT("compressorOut:");
  DEBUG_ECH_PRINT(compressorOut);
  DEBUG_ECH_PRINT(" pumpOut:");
  DEBUG_ECH_PRINT(pumpOut);
  DEBUG_ECH_PRINT(" valveOut:");
  DEBUG_ECH_PRINT(valveOut);
  DEBUG_ECH_PRINT(" valveOut:");
  DEBUG_ECH_PRINTLN(valveOut);
}

void loopECH() {
}




#define ADDR_FAILURE    1213
//////////////////////////////////1213
#define MASK_MAXIMUM_PRESSURE_SWITCH  0x02
#define MASK_MINIMUM_PRESSURE_SWITCH  0x04
#define MASK_FLOW_SWITCH              0x08
#define MASK_FAN_THERMAL_PROTECTION   0x10
#define MASK_ANTIFREEZE_ALARM         0x20
//////////////////////////////////1214
#define MASK_CONF_ERROR           0x01
#define MASK_T_WATER_INFLOWING    0x02
#define MASK_T_WATER_OUTFLOWING   0x04
#define MASK_T_DEFROZE            0x08
#define MASK_T_EXTERNAL           0x10
#define MASK_T_INLET_HIGH         0x20
#define MASK_T_COMPRESSOR         0x40
