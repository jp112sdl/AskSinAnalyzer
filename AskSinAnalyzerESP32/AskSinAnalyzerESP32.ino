//- -----------------------------------------------------------------------------------------------------------------------
// AskSinAnalyzer
// 2016-10-31 papa Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
// 2019-06-01 jp112sdl Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
// 2019-06-01 psi-4ward Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------

#define USE_DISPLAY
#define WEB_BRANCH            "dev_fhem"                  //only changed for development
const String CCU_SV         = "AskSinAnalyzerDevList";  //name of the used system variable on the CCU containing the device list
// #define NDEBUG //No DEBUG -> no output
#define VDEBUG //Verbose DEBUG -> more output

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
#include "RingBuffer.h"

#define VERSION_UPPER "3"
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
enum Screens { TELEGRAM_LIST, RSSI_TEXT, RSSI_GRAPHIC, INFO };
uint8_t currentScreen = TELEGRAM_LIST;
uint16_t currentCircleColor = ILI9341_RED;
#endif

#define CSV_FILENAME                "/log.csv"
#define CSV_HEADER                  "num;time;rssi;fromaddress;from;toaddress;to;len;cnt;typ;flags;msg;"

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

enum BackendTypes { BT_CCU, BT_OTHER };
struct _HomeMaticConfig {
  char ccuIP[IPSIZE]            = "";
  uint8_t backendType           = BT_CCU;
  char backendUrl[VARIABLESIZE] = "";
} HomeMaticConfig;

struct _RSSIConfig {
  uint8_t histogramBarWidth = 5;
} RSSIConfig;

#define MAX_LOG_ENTRIES      51
#define MAX_RSSILOG_ENTRIES 255
#define SIZE_ADDRESS       (6+1)    // address has 6 chars
#define SIZE_SERIAL        (10+1)   // serial has 10 chars
#define SIZE_TYPE            32
#define SIZE_FLAGS           32
#define SIZE_MSG             128

struct _LogTable {
  uint32_t lognumber                  = 0;
  uint8_t  len                        = 0;
  uint8_t  cnt                        = 0;
  time_t   time                       = 0;
  int      rssi                       = -255;
  char     fromSerial [SIZE_SERIAL];
  char     toSerial   [SIZE_SERIAL];
  char     fromAddress[SIZE_ADDRESS];
  char     toAddress  [SIZE_ADDRESS];
  char     typ        [SIZE_TYPE];
  char     flags      [SIZE_FLAGS];
  char     msg        [SIZE_MSG];
};
RingStack<_LogTable,MAX_LOG_ENTRIES> LogTable;

//uint16_t   logLength                  = 0;
uint16_t   logLengthDisplay           = 0;

enum RssiTypes { RSSITYPE_NONE, RSSITYPE_HMRF, RSSITYPE_HMIP };
struct _RSSILogTable {
  time_t   time                       = 0;
  int      rssi                       = -255;
  uint8_t  type                       = RSSITYPE_NONE;
};
RingStack<_RSSILogTable,MAX_RSSILOG_ENTRIES> RSSILogTable;

//uint16_t   rssiLogLength                  = 0;
bool       rssiValueAdded                 = false;

struct _SerialBuffer {
  String   Msg            = "";
  time_t   t              = 0;
} SerialBuffer[255];
uint8_t  msgBufferCount = 0;

JsonArray devices;

uint32_t allCount              = 0;
unsigned long lastDebugMillis  = 0;
bool     updating              = false;
bool     formatfs              = false;
bool     isOnline              = false;
bool     timeOK                = false;
bool     SPIFFSAvailable       = false;
bool     sdAvailable           = false;
bool     startWifiManager      = false;
bool     ONLINE_MODE           = false;
bool     RESOLVE_ADDRESS       = true;
bool     msgBufferProcessing   = true;
uint8_t  DISPLAY_LOG_LINES     = 15;
time_t   bootTime              = 0;
String   updateUrl             = "https://raw.githubusercontent.com/jp112sdl/AskSinAnalyzer/master/ota/AskSinAnalyzerESP32.bin";

#include "Config.h"
#include "NTP.h"
#include "Display.h"
#include "Helper.h"
#include "File.h"
#include "Web.h"
#include "WManager.h"
#include "SerialIn.h"

void setup() {
  DINIT(57600, F("\nASKSINANALYZER ESP32 " VERSION_UPPER "." VERSION_LOWER " (" __DATE__ " " __TIME__ ")\n--------------------------------"));
  pinMode(SD_CS, OUTPUT);
  Serial1.begin(EXTSERIALBAUDRATE, SERIAL_8N1, EXTSERIALRX_PIN, EXTSERIALTX_PIN);
  Serial1.setRxBufferSize(2048);

  pinMode(START_WIFIMANAGER_PIN, INPUT_PULLUP);
  pinMode(SHOW_DISPLAY_LINES_PIN, INPUT_PULLUP);
  pinMode(SHOW_DISPLAY_DETAILS_PIN, INPUT_PULLUP);
  pinMode(ONLINE_MODE_PIN, INPUT_PULLUP);
  pinMode(AP_MODE_LED_PIN, OUTPUT);

  ONLINE_MODE = digitalRead(ONLINE_MODE_PIN) == LOW;
  DPRINT(F("- INIT ONLINE_MODE (")); DPRINTLN(ONLINE_MODE == true ? "Enabled)" : "Disabled)");

  sdAvailable = SdInit();
  DPRINT(F("- INIT SD CARD DONE. SD CARD IS ")); DPRINTLN(sdAvailable ? "AVAILABLE" : "NOT AVAILABLE");

  SPIFFSAvailable = initSPIFFS();
  DPRINT(F("- INIT SPIFFS  DONE. SPIFFS  IS ")); DPRINTLN(SPIFFSAvailable ? "AVAILABLE" : "NOT AVAILABLE");
  initSessionLogOnSPIFFS();

#ifdef USE_DISPLAY
  initTFT();
#endif

  //initLogTables();

  if (ONLINE_MODE) {
    if (!loadSystemConfig()) startWifiManager = true;

    DPRINTLN(F("- Config-Modus durch bootConfigMode aktivieren? "));
    if (bootConfigMode() == true) {
      bootConfigMode(false);
      startWifiManager = true;
      DPRINTLN(F(" -> bootConfigMode aktiviert, starte Config-Modus"));
    } else {
      DPRINTLN(F(" -> bootConfigMode existiert NICHT"));
    }

    startWifiManager |= (digitalRead(START_WIFIMANAGER_PIN) == LOW);

    RESOLVE_ADDRESS = isNotEmpty(HomeMaticConfig.ccuIP);
    DPRINT(F("- RESOLVE_ADDRESS is")); DPRINT(RESOLVE_ADDRESS ? " " : " NOT "); DPRINTLN(F("active!"));

    isOnline = doWifiConnect();
    DPRINT(F("- INIT WIFI CONNECT DONE. WIFI IS ")); DPRINTLN(isOnline ? "AVAILABLE" : "NOT AVAILABLE");
    bool mdns_ok = initmDNS();
    DPRINT(F("- INIT MDNS DONE. STATUS: ")); DPRINTLN(mdns_ok == true ? "OK" : "FAIL");
    timeOK = doNTPinit();
    bootTime = timeOK ? now() : 0;
    DPRINT(F("- INIT NTP DONE.          NTP IS "));   DPRINTLN(timeOK ? "AVAILABLE (" + getDatum(now()) + " " + getUhrzeit(now()) + ")" : "NOT AVAILABLE");
    initWebServer();
    DPRINTLN(F("- INIT WEBSERVER DONE."));
    createJSONDevList(fetchAskSinAnalyzerDevList());
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

  if (ONLINE_MODE) {
    checkWifi();
    checkUpdate(updateUrl);
  }

  if (msgBufferProcessing == true && updating == false) {
    if (formatfs) {
      DPRINT(F("Formatting SPIFFS... "));
      formatfs = false;
      disableLoopWDT();
      WiFi.disconnect();
      SPIFFS.format();
      initSessionLogOnSPIFFS();
      enableLoopWDT();
      DPRINTLN(F("DONE"));
    }

    receiveMessages();

    if (msgBufferCount > 0) {
      for (uint8_t b = 0; b < msgBufferCount; b++) {
        bool isTelegram = fillLogTable(SerialBuffer[b], b);
        if (isTelegram && logLengthDisplay < DISPLAY_LOG_LINES) logLengthDisplay++;
      }
      msgBufferCount = 0;
    }

#ifdef USE_DISPLAY

    static uint32_t last_allCount = 0;

    static bool last_pinstate = HIGH;
    static bool screenChanged = true;

    //check for button press and set screen

    if (digitalRead(START_WIFIMANAGER_PIN) == LOW && last_pinstate == HIGH) {
      delay(100); //debounce
      last_pinstate = LOW;
      DPRINT(F("SWITCH SCREEN TO "));
      switch (currentScreen) {
        case TELEGRAM_LIST:
          currentScreen = RSSI_TEXT;
          DPRINTLN(F("RSSI_TEXT"));
          break;
        case RSSI_TEXT:
          currentScreen = RSSI_GRAPHIC;
          DPRINTLN(F("RSSI_GRAPHIC"));
          break;
        case RSSI_GRAPHIC:
          currentScreen = ONLINE_MODE ? INFO : TELEGRAM_LIST;
          DPRINTLN(F(ONLINE_MODE ? "INFO" : "TELEGRAM_LIST"));
          break;
        case INFO:
          currentScreen = TELEGRAM_LIST;
          DPRINTLN(F("TELEGRAM_LIST"));
          break;
      }
      screenChanged = true;
    } else if (digitalRead(START_WIFIMANAGER_PIN) == HIGH)
      last_pinstate = HIGH;

    //show selected screen
    switch (currentScreen) {
      case TELEGRAM_LIST:
        if (screenChanged) refreshDisplayLog(true);
        if (allCount != last_allCount) {
          refreshDisplayLog(false);
        }
        break;
      case RSSI_TEXT:
        showRSSI_TEXTDisplay(screenChanged);
        break;
      case RSSI_GRAPHIC:
        showRSSI_GRAPHICDisplay(screenChanged);
        break;
      case INFO:
        showInfoDisplay(screenChanged);
        break;
    }

    screenChanged = false;
    last_allCount = allCount;
#endif
  }
}
