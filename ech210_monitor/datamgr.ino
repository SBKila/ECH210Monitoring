/*#define DEBUG_DAT*/
#ifdef DEBUG_DAT
 #define DEBUG_DAT_PRINT(x)  Serial.print (x)
 #define DEBUG_DAT_PRINTLN(x)  Serial.println (x)
 #define DEBUG_DAT_PRINT_DEC(x)  Serial.print (x,DEC)
 #define DEBUG_DAT_PRINTLN_DEC(x)  Serial.println (x,DEC)
#else
 #define DEBUG_DAT_PRINT(x)
 #define DEBUG_DAT_PRINTLN(x)
 #define DEBUG_DAT_PRINT_DEC(x)
 #define DEBUG_DAT_PRINTLN_DEC(x)
#endif
#define DELTA_TEMP 5
#define DELTA_HUMIDITY 10
#define UNDEFINED -600
sint16 sd[8] = {UNDEFINED,UNDEFINED,UNDEFINED,UNDEFINED,UNDEFINED,UNDEFINED,UNDEFINED,UNDEFINED}; 
boolean dataUpdated=false;

boolean isDataUpdated() {
  return dataUpdated;
}
void resetDataUpdated(){
  dataUpdated=false;
}

void setValue(short index,sint16 value){
  DEBUG_DAT_PRINT("SD");
  DEBUG_DAT_PRINT(1+index);
  if( (UNDEFINED == sd[index]) || (sd[index] != value)){
    sd[index]=value;
    dataUpdated=true;
    
    DEBUG_DAT_PRINT(" updated to ");
    DEBUG_DAT_PRINTLN_DEC(value);
  } else {
    DEBUG_DAT_PRINTLN(" not updated.");
  }
}

void setSD(short index,sint16 value,int delta = DELTA_TEMP){
  DEBUG_DAT_PRINT("SD");
  DEBUG_DAT_PRINT(1+index);
  if( (UNDEFINED == sd[index]) || ((abs(sd[index]-value)>delta))){
    sd[index]=value;
    dataUpdated=true;
    
    DEBUG_DAT_PRINT(" updated to ");
    DEBUG_DAT_PRINTLN_DEC(value);
  } else {
    DEBUG_DAT_PRINTLN(" not updated.");
  }
}

void setSD1(sint16 value){
 setSD(0,value);
}
sint16 getSD1(){
   return sd[0];
}

void setSD2(sint16 value){
 setSD(1,value);
}
sint16 getSD2(){
   return sd[1];
}
void setSD3(sint16 value){
 setSD(2,value);
}
sint16 getSD3(){
   return sd[2];
}
void setSD4(sint16 value){
 setSD(3,value);
}
sint16 getSD4(){
   return sd[3];
}
void setTemperature(int value){
  setSD(4,value);
}
int getTemperature(){
  return sd[4];
}
void setHumidity(int value){
  setSD(5,value,DELTA_HUMIDITY);
}
int getHumidity(){
  return sd[5];
}
void setDigitalOutput(sint16 value){
  setValue(6,value);
  //setValue(6,((value&0xFF00)&&(sd[6]&0x00FF)));
}
sint16 getDigitalOutput() {
   return sd[6];
}
void setDigitalInput(sint16 value){
  setValue(7,value);
  //setValue(6,(((value>>8)&0x00FF)&&(sd[6]&0xFF00)));
}
sint16 getDigitalInput() {
   return sd[7];
}
