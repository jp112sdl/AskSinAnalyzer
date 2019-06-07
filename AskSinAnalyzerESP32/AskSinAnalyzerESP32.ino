#define USE_DISPLAY

#include <Preferences.h>
#include <esp_bt_main.h>
#include <esp_bt.h>
#include "WM.h"
#include <HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <TimeLib.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include "FS.h"
#include "SPIFFS.h"
#include <SD.h>
#include <Wire.h>
#ifdef USE_DISPLAY
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <U8g2_for_Adafruit_GFX.h>
#endif

//Pin definitions for external switches
#define START_WIFIMANAGER_PIN    15
#define SHOW_DISPLAY_LINES_PIN   13
#define SHOW_DISPLAY_DETAILS_PIN 12
#define ONLINE_MODE_PIN          14

//Pin definition for LED
#define AP_MODE_LED_PIN          32

#define SD_CS                    27

//Pin definitions for serial connection to AskSinSniffer
#define EXTSERIALTX_PIN          17
#define EXTSERIALRX_PIN          16
#define EXTSERIALBAUDRATE        57600

#ifdef USE_DISPLAY
#define TFT_LED                 33
#define TFT_CS                   5
#define TFT_RST                 26
#define TFT_DC                  25
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
U8G2_FOR_ADAFRUIT_GFX u8g;

#define DISPLAY_LOG_LINE_HEIGHT  15
#define DISPLAY_LOG_OFFSET_TOP   27
#endif

#define CSV_FILENAME                "/log.csv"
#define CONFIG_FILENAME             "/config.json"
#define BOOTCONFIGMODE_FILENAME     "/bootcfg.mod"

#define CSV_HEADER                  "num;time;rssi;from;to;len;cnt;typ;flags;"

#define IPSIZE                16
struct _NetConfig {
  char ip[IPSIZE]      = "0.0.0.0";
  char netmask[IPSIZE] = "0.0.0.0";
  char gw[IPSIZE]      = "0.0.0.0";
} NetConfig;

#define VARIABLESIZE         255
struct _HomeMaticConfig {
  char ccuIP[IPSIZE]   = "";
  char SVAnalyzeInput[VARIABLESIZE] = "";
  char SVAnalyzeOutput[VARIABLESIZE] = "";
} HomeMaticConfig;

#define ADDRESSTABLE_LENGTH 512
struct _AddressTable {
  String Address = "";
  String Serial = "";
} AddressTable[ADDRESSTABLE_LENGTH];
uint16_t AddressTableCount = 0;

#define MAX_LOG_ENTRIES 200
struct _LogTable {
  uint32_t lognumber = 0;
  char from[11];
  char to[11];
  int rssi = -255;
  uint8_t len = 0;
  uint8_t cnt = 0;
  char typ[32];
  char flags[32];
  time_t time = 0;
} LogTable[MAX_LOG_ENTRIES];
uint16_t logLength = 0;
uint16_t logLengthDisplay = 0;

String   msgBuffer[255];
uint8_t  msgBufferCount    = 0;
uint32_t allCount          = 0;
bool     isOnline          = false;
bool     timeOK            = false;
bool     spiffsAvailable   = false;
bool     sdAvailable       = false;
bool     startWifiManager  = false;
bool     ONLINE_MODE       = false;
bool     RESOLVE_ADDRESS   = true;
uint8_t  DISPLAY_LOG_LINES = 15;

#include "Config.h"
#include "SDFunctions.h"
#include "File.h"
#include "Display.h"
#include "NTP.h"
#include "CCUFunctions.h"
#include "Helper.h"
#include "SerialIn.h"
#include "Web.h"
#include "WManager.h"

void setup() {
  Serial.begin(57600);
  Serial.println();
  pinMode(SD_CS, OUTPUT);
  Serial1.begin(EXTSERIALBAUDRATE, SERIAL_8N1, EXTSERIALRX_PIN, EXTSERIALTX_PIN);

  pinMode(START_WIFIMANAGER_PIN, INPUT_PULLUP);
  pinMode(SHOW_DISPLAY_LINES_PIN, INPUT_PULLUP);
  pinMode(SHOW_DISPLAY_DETAILS_PIN, INPUT_PULLUP);
  pinMode(ONLINE_MODE_PIN, INPUT_PULLUP);
  pinMode(AP_MODE_LED_PIN, OUTPUT);

  ONLINE_MODE = digitalRead(ONLINE_MODE_PIN) == LOW;

  for (uint16_t c = 0; c < ADDRESSTABLE_LENGTH; c++) {
    AddressTable[c].Address = "";
    AddressTable[c].Serial = "";
  }

  sdAvailable = SdInit();

#ifdef USE_DISPLAY
  initTFT();
#endif

  initLogTable();

  if (ONLINE_MODE) {
    Serial.println(F("Config-Modus durch bootConfigMode aktivieren? "));
    if (SPIFFS.begin()) {
      Serial.println(F("-> bootConfigModeFilename mounted file system"));
      if (SPIFFS.exists(BOOTCONFIGMODE_FILENAME)) {
        startWifiManager = true;
        Serial.println("-> " + String(BOOTCONFIGMODE_FILENAME) + " existiert, starte Config-Modus");
        SPIFFS.remove(BOOTCONFIGMODE_FILENAME);
        SPIFFS.end();
      } else {
        Serial.println("-> " + String(BOOTCONFIGMODE_FILENAME) + " existiert NICHT");
      }
    } else {
      Serial.println(F("-> Nein, SPIFFS mount fail!"));
    }

    if (!loadSystemConfig()) startWifiManager = true;
    //Serial.println("startWifiManager = " + String(startWifiManager));

    startWifiManager |= (digitalRead(START_WIFIMANAGER_PIN) == LOW);
    //Serial.println("startWifiManager = " + String(startWifiManager));
    RESOLVE_ADDRESS = (HomeMaticConfig.ccuIP != "" && HomeMaticConfig.SVAnalyzeInput != "" && HomeMaticConfig.SVAnalyzeOutput != "");

    deInitBLE();
    isOnline = doWifiConnect();
    timeOK = doNTPinit();
    initWebServer();
  }

  spiffsAvailable = initSPIFFS();
#ifdef USE_DISPLAY
  drawStatusCircle(ILI9341_GREEN);
#endif
}

void loop() {
  receiveMessages();

  if (ONLINE_MODE)
    checkWifi();

  if (msgBufferCount > 0) {
    for (uint8_t b = 0; b < msgBufferCount; b++) {
      fillLogTable(msgBuffer[b], b);
#ifdef USE_DISPLAY
      refreshDisplayLog();
#endif
    }
    msgBufferCount = 0;
  }
}

