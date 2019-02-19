/*#define DEBUG_MOD*/
#ifdef DEBUG_MOD
#define DEBUG_MOD_PRINT(x)  Serial.print (x)
#define DEBUG_MOD_PRINTLN(x)  Serial.println (x)
#define DEBUG_MOD_PRINT_NUM(x,y)  Serial.print (x,y)
#define DEBUG_MOD_PRINTLN_NUM(x,y)  Serial.println (x,y)
#else
#define DEBUG_MOD_PRINT(x)
#define DEBUG_MOD_PRINTLN(x)
#define DEBUG_MOD_PRINT_NUM(x,y)
#define DEBUG_MOD_PRINTLN_NUM(x,y)
#endif

#include "SoftwareSerial8E1.h"
// Modbus RTU pins   D7(13),D8(15)   RX,TX
SoftwareSerial8E1 swSer(MODBUS_RX_PIN, MODBUS_TX_PIN);


uint16_t _u16ReadAddress;                                    ///< slave register from which to read
uint16_t _u16ReadQty;                                        ///< quantity of words to read
uint8_t  _u8SerialPort;                                      ///< serial port (0..3) initialized in constructor
uint8_t  _u8MBSlave;                                         ///< Modbus slave (1..255) initialized in constructor
const uint8_t ku8MaxBufferSize                = 64;          ///< size of response/transmit buffers
uint16_t _u16ResponseBuffer[ku8MaxBufferSize];               ///< buffer to store Modbus slave response; read via GetResponseBuffer()

uint8_t _u8ResponseBufferIndex;
uint8_t _u8ResponseBufferLength;

// Modbus timeout [milliseconds]
const uint8_t ku8MBResponseTimeout            = 200;  ///< Modbus timeout [milliseconds]

uint8_t _u8TransmitBufferIndex;
uint16_t u16TransmitBufferLength;
const uint8_t ku8MBReadHoldingRegisters       = 0x03; ///< Modbus function 0x03 Read Holding Registers
// const uint8_t _ku8MBReadInputRegisters         = 0x04; ///< Modbus function 0x04 Read Input Registers


/*
   Modbus transaction was successful; the following checks were valid:
      - slave ID
      - function code
      - response code
      - data
      - CRC

    @ingroup constant
*/
static const uint8_t ku8MBSuccess                    = 0x00;
static const uint8_t ku8MBInvalidSlaveID             = 0xE0;
static const uint8_t ku8MBInvalidFunction            = 0xE1;
static const uint8_t ku8MBResponseTimedOut           = 0xE2;
static const uint8_t ku8MBInvalidCRC                 = 0xE3;


/**
  CRC ESP8266

  Creates class object using default serial port 0, specified Modbus slave ID.

  @overload void ModbusMaster::ModbusMaster(uint8_t u8MBSlave)
  @param u8MBSlave Modbus slave ID (1..255)
  @ingroup setup
*/

//Function  crc16 created for ESP8266   - PDAControl
//http://www.atmel.com/webdoc/AVRLibcReferenceManual/group__util__crc_1ga95371c87f25b0a2497d9cba13190847f.html
// append CRC

uint16_t _crc16_update2 (uint16_t crc, uint8_t a)
{
  int i;

  crc ^= a;
  for (i = 0; i < 8; ++i)
  {
    if (crc & 1)
      crc = (crc >> 1) ^ 0xA001;
    else
      crc = (crc >> 1);
  }

  return crc;
}
// idle callback function; gets called during idle time between TX and RX
void (*_idle)();

/**
  Creates class object using default serial port 0, specified Modbus slave ID.

  @overload void ModbusMaster::ModbusMaster(uint8_t u8MBSlave)
  @param u8MBSlave Modbus slave ID (1..255)
  @ingroup setup
*/
void ESP8266ModbusMaster232_init(uint8_t u8MBSlave) {
  // define pin modes for tx, rx:
  pinMode(MODBUS_RX_PIN, INPUT);
  pinMode(MODBUS_TX_PIN, OUTPUT);
  _u8SerialPort = 0;
  DEBUG_MOD_PRINT_NUM(u8MBSlave,HEX);
  _u8MBSlave = u8MBSlave;
}

/**
  Initialize class object.

  Sets up the swSer port using specified baud rate.
  Call once class has been instantiated, typically within setup().

  @overload ModbusMaster::begin(uint16_t u16BaudRate)
  @param u16BaudRate baud rate, in standard increments (300..115200)
  @ingroup setup
*/
void ESP8266ModbusMaster232_begin(unsigned long BaudRate )
{
  //  txBuffer = (uint16_t*) calloc(ku8MaxBufferSize, sizeof(uint16_t));
  _u8TransmitBufferIndex = 0;
  u16TransmitBufferLength = 0;

  delay(100);
  swSer.begin(BaudRate);
}

/**
  Modbus function 0x03 Read Holding Registers.

  This function code is used to read the contents of a contiguous block of
  holding registers in a remote device. The request specifies the starting
  register address and the number of registers. Registers are addressed
  starting at zero.

  The register data in the response buffer is packed as one word per
  register.

  @param u16ReadAddress address of the first holding register (0x0000..0xFFFF)
  @param u16ReadQty quantity of holding registers to read (1..125, enforced by remote device)
  @return 0 on success; exception number on failure
  @ingroup register
*/
uint8_t ESP8266ModbusMaster232_readHoldingRegisters(uint16_t u16ReadAddress,
    uint16_t u16ReadQty)
{
  _u16ReadAddress = u16ReadAddress;
  _u16ReadQty = u16ReadQty;
  return ModbusMasterTransaction(ku8MBReadHoldingRegisters);
}


/**
  Retrieve data from response buffer.

  @see ModbusMaster::clearResponseBuffer()
  @param u8Index index of response buffer array (0x00..0x3F)
  @return value in position u8Index of response buffer (0x0000..0xFFFF)
  @ingroup buffer
*/
uint16_t ESP8266ModbusMaster232_getResponseBuffer(uint8_t u8Index)
{
  if (u8Index < ku8MaxBufferSize)
  {
    return _u16ResponseBuffer[u8Index];
  }
  else
  {
    return 0xFFFF;
  }
}


/**
  Clear Modbus response buffer.

  @see ModbusMaster::getResponseBuffer(uint8_t u8Index)
  @ingroup buffer
*/
void ESP8266ModbusMaster232_clearResponseBuffer()
{
  uint8_t i;

  for (i = 0; i < ku8MaxBufferSize; i++)
  {
    _u16ResponseBuffer[i] = 0;
  }
}



/* _____PRIVATE FUNCTIONS____________________________________________________ */
/**
  Modbus transaction engine.
  Sequence:
  - assemble Modbus Request Application Data Unit (ADU),
    based on particular function called
  - transmit request over selected serial port
  - wait for/retrieve response
  - evaluate/disassemble response
  - return status (success/exception)

  @param u8MBFunction Modbus function (0x01..0xFF)
  @return 0 on success; exception number on failure
*/
uint8_t ModbusMasterTransaction(uint8_t u8MBFunction)
{
  uint8_t u8ModbusADU[256];
  uint8_t u8ModbusADUSize = 0;
  uint8_t i, u8Qty;
  uint16_t u16CRC;
  uint32_t u32StartTime;
  uint8_t u8BytesLeft = 8;
  uint8_t u8MBStatus = ku8MBSuccess;

  // assemble Modbus Request Application Data Unit
  u8ModbusADU[u8ModbusADUSize++] = _u8MBSlave;
  u8ModbusADU[u8ModbusADUSize++] = u8MBFunction;

  switch (u8MBFunction)
  {
    case ku8MBReadHoldingRegisters:
      u8ModbusADU[u8ModbusADUSize++] = highByte(_u16ReadAddress);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16ReadAddress);
      u8ModbusADU[u8ModbusADUSize++] = highByte(_u16ReadQty);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16ReadQty);
      break;
  }

  u16CRC = 0xFFFF;
  for (i = 0; i < u8ModbusADUSize; i++)
  {
    //Function  crc16  for ESP8266   - PDAControl
    u16CRC = _crc16_update2(u16CRC, u8ModbusADU[i]);
  }
  u8ModbusADU[u8ModbusADUSize++] = lowByte(u16CRC);
  u8ModbusADU[u8ModbusADUSize++] = highByte(u16CRC);
  u8ModbusADU[u8ModbusADUSize] = 0;


  DEBUG_MOD_PRINT("Sending: ");
  // transmit request
  for (i = 0; i < u8ModbusADUSize; i++)
  {
    DEBUG_MOD_PRINT_NUM(u8ModbusADU[i], HEX);
    DEBUG_MOD_PRINT("_");
    swSer.print(char(u8ModbusADU[i]));
  }
  DEBUG_MOD_PRINTLN("");

  delay(2);

  u8ModbusADUSize = 0;

  DEBUG_MOD_PRINT("Receiving: ");
  // loop until we run out of time or bytes, or an error occurs
  u32StartTime = millis();

  
  // wait until modbus frame arrive
  int val = 0xFF;
  long cont = 0;
  while ((val != _u8MBSlave) && (cont < 1000)) {
    val = swSer.read();
    delay(5);
    cont ++;
  }

  if (cont == 1000) {
    DEBUG_MOD_PRINTLN("slave is nute");
    u8MBStatus = ku8MBResponseTimedOut;
  } else {
    DEBUG_MOD_PRINT_NUM(val, HEX);
    DEBUG_MOD_PRINT("_");
    u8ModbusADU[u8ModbusADUSize++] = val;
  }

    
  while (u8BytesLeft && !u8MBStatus)
  {
    if (swSer.available()) {
      
      u8ModbusADU[u8ModbusADUSize] = swSer.read();
      DEBUG_MOD_PRINT_NUM(u8ModbusADU[u8ModbusADUSize], HEX);
      DEBUG_MOD_PRINT("_");
      
      u8BytesLeft--;
      u8ModbusADUSize ++;

      // evaluate slave ID, function code once enough bytes have been read
      if (u8ModbusADUSize == 5) {
        
        // verify response is for correct Modbus slave
        if (u8ModbusADU[0] != _u8MBSlave) {
          u8MBStatus = ku8MBInvalidSlaveID;
          break;
        }
        
        // verify response is for correct Modbus function code (mask exception bit 7)
        if ((u8ModbusADU[1] & 0x7F) != u8MBFunction) {
          u8MBStatus = ku8MBInvalidFunction;
          break;
        }
        
        // check whether Modbus exception occurred; return Modbus Exception Code
        if (bitRead(u8ModbusADU[1], 7)) {
          u8MBStatus = u8ModbusADU[2];
          break;
        }
        
        // evaluate returned Modbus function code
        switch (u8ModbusADU[1]) {
          case ku8MBReadHoldingRegisters:
            u8BytesLeft = u8ModbusADU[2];
            break;
        }
      }
    }
    
    if (millis() > (u32StartTime + ku8MBResponseTimeout)) {
      u8MBStatus = ku8MBResponseTimedOut;
    }
  }
  DEBUG_MOD_PRINTLN("");

  // verify response is large enough to inspect further
  if (!u8MBStatus && u8ModbusADUSize >= 5)
  {
    // calculate CRC
    u16CRC = 0xFFFF;
    for (i = 0; i < (u8ModbusADUSize - 2); i++)
    {
      //Function  crc16  for ESP8266   - PDAControl
      u16CRC = _crc16_update2(u16CRC, u8ModbusADU[i]);
    }

    // verify CRC
    if (!u8MBStatus && (lowByte(u16CRC) != u8ModbusADU[u8ModbusADUSize - 2] ||
                        highByte(u16CRC) != u8ModbusADU[u8ModbusADUSize - 1]))
    {
      u8MBStatus = ku8MBInvalidCRC;
    }
  }

  // disassemble ADU into words
  if (!u8MBStatus)
  {
    // evaluate returned Modbus function code
    switch (u8ModbusADU[1])
    {
      case ku8MBReadHoldingRegisters:
        // load bytes into word; response bytes are ordered H, L, H, L, ...
        for (i = 0; i < (u8ModbusADU[2] >> 1); i++)
        {
          if (i < ku8MaxBufferSize)
          {
            //Function makeWord Modified to ESP8266 - PDAControl

            // _u16ResponseBuffer[i] = makeWord(u8ModbusADU[2 * i + 3], u8ModbusADU[2 * i + 4]);

            //return (h << 8) | l;
            _u16ResponseBuffer[i] = (u8ModbusADU[2 * i + 3] << 8) | u8ModbusADU[2 * i + 4];
          }

          _u8ResponseBufferLength = i;
        }
        break;
    }
  }


  _u8TransmitBufferIndex = 0;
  u16TransmitBufferLength = 0;
  _u8ResponseBufferIndex = 0;

  return u8MBStatus;
}
