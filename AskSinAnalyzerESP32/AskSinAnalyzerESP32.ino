#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <U8g2_for_Adafruit_GFX.h>
#include "settings.h"

#define SHOW_DISPLAY_LINES
#define SHOW_DETAILS
#define ONLINE_MODE

#ifdef SHOW_DETAILS
#define LOG_BLOCK_SIZE  3
#else
#define LOG_BLOCK_SIZE  1
#endif

#define TFT_LED                 33
#define TFT_CS                   5
#define TFT_RST                 26
#define TFT_DC                  25
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
U8G2_FOR_ADAFRUIT_GFX u8g;

#define DISPLAY_LOG_LINES        15 / LOG_BLOCK_SIZE
#define DISPLAY_LOG_LINE_HEIGHT  15
#define DISPLAY_LOG_OFFSET_TOP   27

#define EXTSERIALTX_PIN     17
#define EXTSERIALRX_PIN     16
#define EXTSERIALBAUDRATE   57600

#define ADDRESSTABLE_LENGTH 512
struct _AddressTable {
  String Address = "";
  String Serial = "";
} AddressTable[ADDRESSTABLE_LENGTH];
uint16_t AddressTableCount = 0;

#define MAX_LOG_ENTRIES 256
struct _LogTable {
  char from[11];
  char to[11];
  int rssi = -255;
  uint8_t len = 0;
  uint8_t cnt = 0;
  char typ[32];
  char flags[32];
} LogTable[MAX_LOG_ENTRIES];
uint16_t logLength = 0;
uint16_t logLengthDisplay = 0;

String msgBuffer[255];
uint8_t msgBufferCount = 0;
uint32_t allCount = 0;
bool isOnline = false;

#include "Display.h"
#include "Wifi.h"
#include "CCUFunctions.h"
#include "Helper.h"
#include "SerialIn.h"

void setup() {
#ifdef ONLINE_MODE
  for (uint16_t c = 0; c < ADDRESSTABLE_LENGTH; c++) {
    AddressTable[c].Address = "";
    AddressTable[c].Serial = "";
  }
#endif
  Serial.begin(57600);
  Serial.println();
  Serial1.begin(EXTSERIALBAUDRATE, SERIAL_8N1, EXTSERIALRX_PIN, EXTSERIALTX_PIN);
  initTFT();
  initLogTable();
#ifdef ONLINE_MODE
  isOnline = wifiConnect();
#endif
  drawStatusCircle(ILI9341_GREEN);
}

void loop() {
  receiveMessages();

  if (msgBufferCount > 0) {
    for (uint8_t b = 0; b < msgBufferCount; b++) {

      fillLogTable(msgBuffer[b]);

      refreshDisplayLog();



      //Serial.println("FROM: " + String(fromStr));
      //Serial.println("TO  : " + String(toStr));
      //Serial.println("RSSI: " + rssiStr);
      //Serial.println("LEN : " + String(lengthStr));
      //Serial.println("CNT : " + String(countStr));
      //Serial.println("TYP : " + String(typ));
      //Serial.println("FLG : " + String(flags));
    }
    msgBufferCount = 0;
  }
}

