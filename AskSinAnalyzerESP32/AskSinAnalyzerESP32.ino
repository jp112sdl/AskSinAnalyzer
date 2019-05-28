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

//cd /etc/config/rfd;grep -i "\" address=\"0x4E92CC\"" *.dev|awk -F"." '{print $1}'


#define EXTSERIALTX_PIN     17
#define EXTSERIALRX_PIN     16
#define EXTSERIALBAUDRATE   57600

#define ADDRESSTABLE_LENGTH 512
struct _AddressTable {
  String Address = "";
  String Serial = "";
} AddressTable[ADDRESSTABLE_LENGTH];
uint16_t AddressTableCount = 0;

struct _LogTable {
  String from = "";
  String to = "";
  int rssi = -255;
  uint8_t len = 0;
  uint8_t cnt = 0;
  String typ = "";
  String flags = "";
} LogTable[DISPLAY_LOG_LINES];
uint16_t logLength = 0;

String msgBuffer[255];
uint8_t msgBufferCount = 0;
uint32_t allCount = 0;
bool isOnline = false;

bool wifiConnect() {
  drawStatusCircle(ILI9341_YELLOW);
  WiFi.begin(WIFI_SSID, WIFI_PSK);
  uint8_t connect_count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
    connect_count++;
    if (connect_count > 60) return false;
  }
  connect_count = 0;
  Serial.println("Connected to the WiFi network");
  return true;
}

void receiveMessages() {
  String inStr = "";
  while (Serial1.available() > 0) {
    char inChar = (char)Serial1.read();
    if (inChar != ';') {
      if (inChar != '\n' && inChar != '\r') {
        inStr += inChar;
      }
    } else {
      //Serial.println("MESSAGE #"+String(msgBufferCount)+" ADDED: "+inStr);
      msgBuffer[msgBufferCount] = inStr;
      inStr = "";
      msgBufferCount++;
      allCount++;
    }
  }
}

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
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);
  u8g.begin(tft);
  tft.setRotation(3);
  u8g.setFontMode(1);
  u8g.setFontDirection(0);
  u8g.setForegroundColor(ILI9341_WHITE);
  u8g.setBackgroundColor(ILI9341_BLACK);
  drawStatusCircle(ILI9341_RED);
  u8g.setFont(u8g2_font_9x15B_mr);
  u8g.setCursor(0, 10);
  u8g.println("   FROM        TO      RSSI LEN CNT");
#ifdef SHOW_DISPLAY_LINES
  tft.drawLine(0, 14, tft.width(), 14, ILI9341_WHITE);
  for (uint8_t c = 0; c < DISPLAY_LOG_LINES; c++)
    tft.drawLine(0, DISPLAY_LOG_OFFSET_TOP + (2 * DISPLAY_LOG_LINE_HEIGHT) + (DISPLAY_LOG_LINE_HEIGHT * LOG_BLOCK_SIZE * c) + 2, tft.width(), DISPLAY_LOG_OFFSET_TOP + (2 * DISPLAY_LOG_LINE_HEIGHT) + (DISPLAY_LOG_LINE_HEIGHT * LOG_BLOCK_SIZE * c) + 2, ILI9341_WHITE);
#endif
#ifdef ONLINE_MODE
  isOnline = wifiConnect();
#endif
  drawStatusCircle(ILI9341_GREEN);
}

void drawStatusCircle(uint16_t color) {
  if (isOnline)
    tft.fillCircle(5, 5, 5, color);
  else
    tft.drawCircle(5, 5, 5, color);
}

String getFlags(String in) {
  int flagsInt =  (strtol(&in[0], NULL, 16) & 0xFF);
  String flags = "";
  if (flagsInt & 0x01) flags += "WKUP ";
  if (flagsInt & 0x02) flags += "WKMEUP ";
  if (flagsInt & 0x04) flags += "BCAST ";
  if (flagsInt & 0x10) flags += "BURST ";
  if (flagsInt & 0x20) flags += "BIDI ";
  if (flagsInt & 0x40) flags += "RPTED ";
  if (flagsInt & 0x80) flags += "RPTEN ";
  if (flags.length() < 30)
    for (uint8_t i = 0; i < (30 - flags.length()); i++) {
      //Serial.println("i="+String(i));
      flags += " ";
    }
  return flags;
}

String getTyp(String in) {
  String typ = "";
  if (in == "00") typ = "DEVINFO";
  else if (in == "01") typ = "CONFIG";
  else if (in == "02") typ = "RESPONSE";
  else if (in == "03") typ = "RESPONSE_AES";
  else if (in == "04") typ = "KEY_EXCHANGE";
  else if (in == "10") typ = "INFO";
  else if (in == "11") typ = "ACTION";
  else if (in == "12") typ = "HAVE_DATA";
  else if (in == "3E") typ = "SWITCH_EVENT";
  else if (in == "3F") typ = "TIMESTAMP";
  else if (in == "40") typ = "REMOTE_EVENT";
  else if (in == "41") typ = "SENSOR_EVENT";
  else if (in == "53") typ = "SENSOR_DATA";
  else if (in == "58") typ = "CLIMATE_EVENT";
  else if (in == "5A") typ = "CLIMATECTRL_EVENT ";
  else if (in == "5E") typ = "POWER_EVENT";
  else if (in == "5F") typ = "POWER_EVENT_CYCLIC";
  else if (in == "70") typ = "WEATHER";
  else typ = in;
  if (typ.length() < 30)
    for (uint8_t i = 0; i < (30 - typ.length()); i++) {
      //Serial.println("i="+String(i));
      typ += " ";
    }
  return typ;
}


bool setCCURequest(String SV, String val) {
  if (WiFi.status() == WL_CONNECTED) {
    drawStatusCircle(ILI9341_BLUE);
    HTTPClient http;
    //http.setTimeout(HTTPTimeOut);
    String url = "http://" + CCU_IP + ":8181/a.exe?ret=dom.GetObject(%22" + SV + "%22).State(%22" + val + "%22)";
    Serial.println("setCCURequest url: " + url);
    http.begin(url);
    int httpCode = http.GET();
    String payload = "";

    if (httpCode > 0) {
      Serial.println("HTTP success");
      payload = http.getString();
    }
    if (httpCode != 200) {
      Serial.println("HTTP failed with HTTP Error Code " + String(httpCode));
    }
    http.end();

    payload = payload.substring(payload.indexOf("<ret>"));
    payload = payload.substring(5, payload.indexOf("</ret>"));

    Serial.println("result: " + payload);
    return (payload != "null");
  } else return false;
}

String getCCURequestResult(String SV) {
  if (WiFi.status() == WL_CONNECTED) {
    drawStatusCircle(ILI9341_BLUE);
    HTTPClient http;
    //http.setTimeout(HTTPTimeOut);
    String url = "http://" + CCU_IP + ":8181/a.exe?ret=dom.GetObject(%22" + SV + "%22).State()";
    Serial.println("getCCURequestResult url: " + url);
    http.begin(url);
    int httpCode = http.GET();
    String payload = "error";
    if (httpCode > 0) {
      payload = http.getString();
    }
    if (httpCode != 200) {
      Serial.println("HTTP fail");
    }
    http.end();

    payload = payload.substring(payload.indexOf("<ret>"));
    payload = payload.substring(5, payload.indexOf("</ret>"));
    Serial.println("result: " + payload);

    return payload;
  } else {
    Serial.println("getCCURequestResult: WiFi.status() != WL_CONNECTED, trying to reconnect");
    return "ERR";
  }
}

String getSerialFromAddress(String in) {
  String out = in;

  if (in != "000000") {
    for (uint16_t c = 0; c < ADDRESSTABLE_LENGTH; c++) {
      if (AddressTable[c].Address == in) {
        Serial.println("FOUND LOCAL");
        drawStatusCircle(ILI9341_GREEN);
        return (AddressTable[c].Serial).substring(0, 10);
      }
    }

    if  (WiFi.status() == WL_CONNECTED) {
      if (setCCURequest(INPUT_SV, in) == true) {
        drawStatusCircle(ILI9341_GREEN);
        delay(250);
        String res = getCCURequestResult(OUTPUT_SV);
        if (res.length() > 7 && res.substring(0, 6) == in) {
          drawStatusCircle(ILI9341_GREEN);
          Serial.println("FOUND VALID RESULT");
          AddressTable[AddressTableCount].Address = in;
          res = res.substring(res.indexOf(","));
          res = res.substring(1);
          res.replace("BidCoS-RF", "<ZENTRALE>");

          Serial.println("SERIAL = " + res);
          AddressTable[AddressTableCount].Serial = res;
          out = res;
          AddressTableCount++;
        } else {
          drawStatusCircle(ILI9341_RED);
        }
      } else {
        drawStatusCircle(ILI9341_RED);
      }
    }
  } else {
    out = "<ALLE>";
  }
  out.trim();
  if (out.length() == 6) out = "  " + out + "  ";
  return out;
}

void loop() {

  receiveMessages();

  if (msgBufferCount > 0) {
    for (uint8_t b = 0; b < msgBufferCount; b++) {
      Serial.println("I " + msgBuffer[b]);


      String rssiIn = msgBuffer[b].substring(1, 3);
      int rssi = -1 * (strtol(&rssiIn[0], NULL, 16) & 0xFF);

      String lengthIn = msgBuffer[b].substring(3, 5);
      uint8_t len = (strtol(&lengthIn[0], NULL, 16) & 0xFF);

      String countIn = msgBuffer[b].substring(5, 7);
      uint8_t cnt = (strtol(&countIn[0], NULL, 16) & 0xFF);

      String flags = getFlags(msgBuffer[b].substring(7, 9));
      String typ = getTyp(msgBuffer[b].substring(9, 11));

#ifdef ONLINE_MODE
      String fromStr = getSerialFromAddress(msgBuffer[b].substring(11, 17));
      String toStr = getSerialFromAddress(msgBuffer[b].substring(17, 23));
#else
      String fromStr = "  " + msgBuffer[b].substring(11, 17) + "  ";
      String toStr = "  " + msgBuffer[b].substring(17, 23) + "  ";
#endif

      //Serial.println("FROM STR Len " + String(fromStr.length()));
      //Serial.println("  TO STR Len " + String(toStr.length()));



      if (logLength > 0) {
        for (uint16_t c = logLength; c > 0; c--) {
          LogTable[c].from = LogTable[c - 1].from;
          LogTable[c].to = LogTable[c - 1].to;
          LogTable[c].rssi = LogTable[c - 1].rssi;
          LogTable[c].len = LogTable[c - 1].len;
          LogTable[c].cnt = LogTable[c - 1].cnt;
          LogTable[c].typ = LogTable[c - 1].typ;
          LogTable[c].flags = LogTable[c - 1].flags;
        }
      }
      LogTable[0].from = fromStr;
      LogTable[0].to = toStr;
      LogTable[0].rssi = rssi;
      LogTable[0].len = len;
      LogTable[0].cnt = cnt;
      LogTable[0].typ = typ;
      LogTable[0].flags = flags;

      for (uint8_t c = 0; c <= logLength; c++) {
        u8g.setCursor(0, DISPLAY_LOG_OFFSET_TOP + (DISPLAY_LOG_LINE_HEIGHT * LOG_BLOCK_SIZE * c));
        u8g.setFont(u8g2_font_9x15_mr);
        u8g.setForegroundColor(ILI9341_WHITE);
        u8g.print(LogTable[c].from + " " + LogTable[c].to + " ");

        String rssiStr = LogTable[c].rssi < -99 ? String(LogTable[c].rssi) : String(LogTable[c].rssi) + " ";
        u8g.setForegroundColor(ILI9341_GREEN);
        if (LogTable[c].rssi < -60 && LogTable[c].rssi >= -80) u8g.setForegroundColor(ILI9341_YELLOW);
        if (LogTable[c].rssi < -80 && LogTable[c].rssi >= -100) u8g.setForegroundColor(ILI9341_ORANGE);
        if (LogTable[c].rssi < -100) u8g.setForegroundColor(ILI9341_RED);
        u8g.print(" " + rssiStr);
        u8g.setForegroundColor(ILI9341_WHITE);
        String lenStr = String(LogTable[c].len);
        if (LogTable[c].len < 10) lenStr = "  " + lenStr;
        if (LogTable[c].len >= 10 && LogTable[c].len < 100) lenStr = " " + lenStr;

        String cntStr = String(LogTable[c].cnt);
        if (LogTable[c].cnt < 10) cntStr = "  " + cntStr;
        if (LogTable[c].cnt >= 10 && LogTable[c].cnt < 100) cntStr = " " + cntStr;
        u8g.print(" " + lenStr + " " + cntStr);

#ifdef SHOW_DETAILS
        u8g.setCursor(0, DISPLAY_LOG_OFFSET_TOP + DISPLAY_LOG_LINE_HEIGHT + (DISPLAY_LOG_LINE_HEIGHT * LOG_BLOCK_SIZE * c));
        u8g.setForegroundColor(ILI9341_DARKGREY);
        u8g.print("   TYP: ");
        u8g.setForegroundColor(ILI9341_CYAN);
        u8g.print(LogTable[c].typ);
        u8g.setCursor(0, DISPLAY_LOG_OFFSET_TOP + (2 * DISPLAY_LOG_LINE_HEIGHT) + (DISPLAY_LOG_LINE_HEIGHT * LOG_BLOCK_SIZE * c));
        u8g.setForegroundColor(ILI9341_DARKGREY);
        u8g.print(" FLAGS: ");
        u8g.setForegroundColor(ILI9341_OLIVE);
        u8g.print(LogTable[c].flags);
#endif
#ifdef SHOW_DISPLAY_LINES
        tft.drawLine(0, DISPLAY_LOG_OFFSET_TOP + (2 * DISPLAY_LOG_LINE_HEIGHT) + (DISPLAY_LOG_LINE_HEIGHT * LOG_BLOCK_SIZE * c) + 2, tft.width(), DISPLAY_LOG_OFFSET_TOP + (2 * DISPLAY_LOG_LINE_HEIGHT) + (DISPLAY_LOG_LINE_HEIGHT * LOG_BLOCK_SIZE * c) + 2, ILI9341_WHITE);
#endif
      }

      u8g.setCursor(78, 10);
      u8g.setForegroundColor(ILI9341_WHITE);
      u8g.setFont(u8g2_font_7x13_mr);
      u8g.print("(#" + String(allCount) + ")");

      if (logLength < DISPLAY_LOG_LINES - 1) logLength++;

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

