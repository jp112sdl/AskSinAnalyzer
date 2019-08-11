//- -----------------------------------------------------------------------------------------------------------------------
// AskSinAnalyzer
// 2016-10-31 papa Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
// 2019-06-01 jp112sdl Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
// 2019-06-01 psi-4ward Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------


#define USE_DISPLAY
// #define NDEBUG

#include "Debug.h"
#include <Preferences.h>
#include "WM.h"
#include <HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <ESP32httpUpdate.h>
#include <ESPmDNS.h>
#include <TimeLib.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <FS.h>
#include <SPIFFS.h>
#include <SD.h>
#include <Wire.h>
#ifdef USE_DISPLAY
#define HAS_DISPLAY 1
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <U8g2_for_Adafruit_GFX.h>
#else
#define HAS_DISPLAY 0
#endif

#define VERSION_UPPER "2"
#define VERSION_LOWER "0"

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

#define CSV_HEADER                  "num;time;rssi;fromaddress;from;toaddress;to;len;cnt;typ;flags;"

#define IPSIZE                16
#define VARIABLESIZE          255
#define DEFAULT_NTP_SERVER    "0.de.pool.ntp.org"
#define DEFAULT_HOSTNAME      "AskSinAnalyzer"

struct _NetConfig {
  char ip[IPSIZE]             = "0.0.0.0";
  char netmask[IPSIZE]        = "0.0.0.0";
  char gw[IPSIZE]             = "0.0.0.0";
  char hostname[VARIABLESIZE] = DEFAULT_HOSTNAME;
  char ntp[VARIABLESIZE]      = DEFAULT_NTP_SERVER;
} NetConfig;

struct _HomeMaticConfig {
  char ccuIP[IPSIZE]   = "";
} HomeMaticConfig;

#define ADDRESSTABLE_LENGTH 256
struct _AddressTable {
  String Address = "";
  String Serial = "";
} AddressTable[ADDRESSTABLE_LENGTH];
uint16_t AddressTableCount = 0;

#define MAX_LOG_ENTRIES 51
#define SIZE_ADDRESS   (6+1)    // address has 6 chars
#define SIZE_SERIAL    (10+1)   // serial has 10 chars
#define SIZE_TYPE       32
#define SIZE_FLAGS      32

struct _LogTable {
  uint32_t lognumber = 0;
  char fromSerial[SIZE_SERIAL];
  char toSerial[SIZE_SERIAL];
  char fromAddress[SIZE_ADDRESS];
  char toAddress[SIZE_ADDRESS];
  int rssi = -255;
  uint8_t len = 0;
  uint8_t cnt = 0;
  char typ[SIZE_TYPE];
  char flags[SIZE_FLAGS];
  time_t time = 0;
} LogTable[MAX_LOG_ENTRIES + 1];
uint16_t logLength = 0;
uint16_t logLengthDisplay = 0;

struct _SerialBuffer {
  String   Msg            = "";
  time_t   t              = 0;
} SerialBuffer[255];
uint8_t  msgBufferCount = 0;
JsonArray devices;

uint32_t allCount              = 0;
unsigned long lastDebugMillis  = 0;
bool     updating              = false;
bool     showInfoDisplayActive = false;
bool     isOnline              = false;
bool     timeOK                = false;
bool     spiffsAvailable       = false;
bool     sdAvailable           = false;
bool     startWifiManager      = false;
bool     ONLINE_MODE           = false;
bool     RESOLVE_ADDRESS       = true;
uint8_t  DISPLAY_LOG_LINES     = 15;
time_t   bootTime              = 0;
String   updateUrl             = "https://raw.githubusercontent.com/jp112sdl/AskSinAnalyzer/master/ota/AskSinAnalyzerESP32.bin";

#include "Config.h"
#include "NTP.h"
#include "SDFunctions.h"
#include "File.h"
#include "Display.h"
#include "CCUFunctions.h"
#include "Helper.h"
#include "SerialIn.h"
#include "Web.h"
#include "WManager.h"

void setup() {
  DINIT(57600, F("\nASKSINANALYZER ESP32 " VERSION_UPPER "." VERSION_LOWER " (" __DATE__ " " __TIME__ ")\n--------------------------------"));
  pinMode(SD_CS, OUTPUT);
  Serial1.begin(EXTSERIALBAUDRATE, SERIAL_8N1, EXTSERIALRX_PIN, EXTSERIALTX_PIN);

  pinMode(START_WIFIMANAGER_PIN, INPUT_PULLUP);
  pinMode(SHOW_DISPLAY_LINES_PIN, INPUT_PULLUP);
  pinMode(SHOW_DISPLAY_DETAILS_PIN, INPUT_PULLUP);
  pinMode(ONLINE_MODE_PIN, INPUT_PULLUP);
  pinMode(AP_MODE_LED_PIN, OUTPUT);

  ONLINE_MODE = digitalRead(ONLINE_MODE_PIN) == LOW;
  DPRINT(F("- INIT ONLINE_MODE (")); DPRINTLN(ONLINE_MODE == true ? "Enabled)" : "Disabled)");

  sdAvailable = SdInit();
  DPRINT(F("- INIT SD CARD DONE. SD CARD IS ")); DPRINTLN(sdAvailable ? "AVAILABLE" : "NOT AVAILABLE");

  spiffsAvailable = initSPIFFS();
  DPRINT(F("- INIT SPIFFS  DONE. SPIFFS  IS ")); DPRINTLN(spiffsAvailable ? "AVAILABLE" : "NOT AVAILABLE");

#ifdef USE_DISPLAY
  initTFT();
#endif

  initLogTable();

  if (ONLINE_MODE) {
    DPRINTLN(F("- Config-Modus durch bootConfigMode aktivieren? "));
    if (spiffsAvailable && SPIFFS.exists(BOOTCONFIGMODE_FILENAME)) {
      startWifiManager = true;
      DPRINTLN(" -> " + String(BOOTCONFIGMODE_FILENAME) + " existiert, starte Config-Modus");
      SPIFFS.remove(BOOTCONFIGMODE_FILENAME);
      SPIFFS.end();
    } else {
      DPRINTLN(" -> " + String(BOOTCONFIGMODE_FILENAME) + " existiert NICHT");
    }

    if (!loadSystemConfig()) startWifiManager = true;

    startWifiManager |= (digitalRead(START_WIFIMANAGER_PIN) == LOW);

    RESOLVE_ADDRESS = isNotEmpty(HomeMaticConfig.ccuIP);
    DPRINT(F("- RESOLVE_ADDRESS is")); DPRINT(RESOLVE_ADDRESS ? " " : " NOT "); DPRINTLN(F("active!"));

    isOnline = doWifiConnect();
    DPRINT(F("- INIT WIFI CONNECT DONE. WIFI IS ")); DPRINTLN(isOnline ? "AVAILABLE" : "NOT AVAILABLE");
    bool mdns_ok = initmDNS();
    DPRINT(F("- INIT MDNS DONE. STATUS: ")); DPRINTLN(mdns_ok == true ? "OK" : "FAIL");
    timeOK = doNTPinit();
    bootTime = timeOK ? now() : 0;
    DPRINT(F("- INIT NTP DONE.          NTP IS "));   DPRINTLN(timeOK ? "AVAILABLE" : "NOT AVAILABLE");
    initWebServer();
    DPRINTLN(F("- INIT WEBSERVER DONE."));
    DPRINT(F("- Fetching DevList... "));
    createJSONDevList();
    DPRINTLN(F("DONE"));
  }

#ifdef USE_DISPLAY
  drawStatusCircle(ILI9341_GREEN);
#endif
  DPRINTLN(F("- INIT COMPLETE.\n--------------------------------"));
}

void loop() {

  if (millis() - lastDebugMillis > 2000) {
    lastDebugMillis = millis();
    //DPRINT(F("Free Heap Size: ")); DDECLN(ESP.getFreeHeap());
  }

  if (updating == false) {
    receiveMessages();

    if (ONLINE_MODE) {
      checkWifi();
      checkUpdate(updateUrl);
    }

#ifdef USE_DISPLAY
    if (ONLINE_MODE && (digitalRead(START_WIFIMANAGER_PIN) == LOW)) {
      if (showInfoDisplayActive == false) {
        showInfoDisplayActive = true;
        showInfoDisplay();
      }
    } else if (showInfoDisplayActive == true && digitalRead(START_WIFIMANAGER_PIN) == HIGH) {
      showInfoDisplayActive = false;
      tft.fillRect(0, 15, tft.width(), tft.height(), ILI9341_BLACK);
      drawRowLines();
      refreshDisplayLog();
    }
#endif

    if (msgBufferCount > 0) {
      for (uint8_t b = 0; b < msgBufferCount; b++) {
        fillLogTable(SerialBuffer[b], b);
#ifdef USE_DISPLAY
        if (logLengthDisplay < DISPLAY_LOG_LINES) logLengthDisplay++;
        if (showInfoDisplayActive == false) {
          refreshDisplayLog();
        }
#endif
      }
      msgBufferCount = 0;
    }
  }
}

