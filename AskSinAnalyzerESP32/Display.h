//- -----------------------------------------------------------------------------------------------------------------------
// AskSinAnalyzer
// 2019-06-01 jp112sdl Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------


#ifndef DISPLAY_H_
#define DISPLAY_H_

#ifdef USE_DISPLAY
bool showDisplayDetails = false;
bool showDisplayLines = false;
uint8_t LOG_BLOCK_SIZE = 1;

void drawStatusCircle(uint16_t color) {
#ifdef USE_DISPLAY
  if (isOnline)
    tft.fillCircle(5, 5, 5, color);
  else
    tft.drawCircle(5, 5, 5, color);
#endif
  currentCircleColor = color;
}

void drawRowLines() {
  if (showDisplayLines == true) {
    tft.drawLine(0, 14, tft.width(), 14, ILI9341_WHITE);
    for (uint8_t c = 0; c < DISPLAY_LOG_LINES; c++) {
      int y = DISPLAY_LOG_OFFSET_TOP + (DISPLAY_LOG_LINE_HEIGHT * LOG_BLOCK_SIZE * (c + 1)) - DISPLAY_LOG_LINE_HEIGHT + 2;
      tft.drawLine(0, y, tft.width(), y, ILI9341_WHITE);
    }
  }
}

void initTFT() {
  DPRINTLN(F("- INIT TFT."));
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);
  showDisplayDetails =  digitalRead(SHOW_DISPLAY_DETAILS_PIN) == LOW;
  showDisplayLines = digitalRead(SHOW_DISPLAY_LINES_PIN) == LOW;
  DPRINT(F(" - showDisplayDetails = ")); DPRINTLN(showDisplayDetails ? "Enabled" : "Disabled");
  DPRINT(F(" - showDisplayLines   = ")); DPRINTLN(showDisplayLines ? "Enabled" : "Disabled");

  LOG_BLOCK_SIZE = (showDisplayDetails == true ? 3 : 1);
  DISPLAY_LOG_LINES /= LOG_BLOCK_SIZE;
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);
  u8g.begin(tft);
  tft.setRotation(3);
  u8g.setFontMode(1);
  u8g.setFontDirection(0);
  u8g.setForegroundColor(ILI9341_WHITE);
  u8g.setBackgroundColor(ILI9341_BLACK);
  DPRINTLN(F("- INIT TFT DONE."));
}

void refreshDisplayLog(bool firstrun) {
  if (firstrun) {
    tft.fillRect(0, 0, tft.width(), tft.height(), ILI9341_BLACK);
    u8g.setFont(u8g2_font_9x15B_mr);
    u8g.setCursor(0, 10);
    u8g.println("   FROM        TO      RSSI LEN CNT");
    u8g.setCursor(78, 10);
    u8g.setForegroundColor(ILI9341_WHITE);
    drawRowLines();
    drawStatusCircle(currentCircleColor);

  }
  for (uint8_t c = 0; c < logLengthDisplay; c++) {
    u8g.setCursor(0, DISPLAY_LOG_OFFSET_TOP + (DISPLAY_LOG_LINE_HEIGHT * LOG_BLOCK_SIZE * c));
    u8g.setFont(u8g2_font_9x15_mr);
    u8g.setForegroundColor( String(LogTable[c].typ).startsWith("HMIP") ? ILI9341_CYAN : ILI9341_WHITE);

    String from = LogTable[c].fromSerial;
    if (from.startsWith("BidCoS-RF") || from.startsWith("HmIP-RF")) from = "-ZENTRALE-";
    u8g.print(from);
    u8g.print(" ");

    String to = LogTable[c].toSerial;
    if (to == "0000000000") to = "  -ALLE-  ";
    else if (to.startsWith("BidCoS-RF") || to.startsWith("HmIP-RF")) to = "-ZENTRALE-";
    u8g.print(to);
    u8g.print(" ");

    u8g.setForegroundColor(ILI9341_WHITE);

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

    if (showDisplayDetails == true) {
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
    }
    if (showDisplayLines == true) {
      int y = DISPLAY_LOG_OFFSET_TOP + (DISPLAY_LOG_LINE_HEIGHT * LOG_BLOCK_SIZE * (c + 1)) - DISPLAY_LOG_LINE_HEIGHT + 2;
      tft.drawLine(0, y, tft.width(), y, ILI9341_WHITE);
    }
  }

  u8g.setForegroundColor(ILI9341_WHITE);
  u8g.setFont(u8g2_font_7x13_mr);
  String allCntStr = String(allCount);
  int16_t allCntStrLen = u8g.getUTF8Width(allCntStr.c_str());
  u8g.setCursor(88 - allCntStrLen / 2, 10);
  u8g.print("(#" + allCntStr + ")");
}


void showInfoDisplay(bool firstrun) {
  if (firstrun) {
    tft.fillRect(0, 0, tft.width(), tft.height(), ILI9341_BLACK);

    u8g.setFont(u8g2_font_7x14_mr);
    u8g.setForegroundColor(ILI9341_WHITE);
    u8g.setCursor(0, 40);

    u8g.print(F("     Mac-Address: ")); u8g.println(WiFi.macAddress()); u8g.setFont(u8g2_font_7x13_mr); u8g.println(); u8g.setFont(u8g2_font_7x14_mr);
    u8g.print(F("            SSID: ")); u8g.println(WiFi.SSID()); u8g.setFont(u8g2_font_7x13_mr); u8g.println(); u8g.setFont(u8g2_font_7x14_mr);
    u8g.print(F("            RSSI: ")); u8g.print(WiFi.RSSI()); u8g.println(F(" dBm")); u8g.setFont(u8g2_font_7x13_mr); u8g.println(); u8g.setFont(u8g2_font_7x14_mr);
    u8g.println();
    u8g.print(F("              IP: ")); u8g.println(WiFi.localIP()); u8g.setFont(u8g2_font_7x13_mr); u8g.println(); u8g.setFont(u8g2_font_7x14_mr);
    u8g.print(F("             NTP: ")); u8g.println(NetConfig.ntp); u8g.setFont(u8g2_font_7x13_mr); u8g.println(); u8g.setFont(u8g2_font_7x14_mr);
    u8g.println();
    u8g.print(F("          CCU IP: ")); u8g.println(HomeMaticConfig.ccuIP); u8g.setFont(u8g2_font_7x13_mr); u8g.println(); u8g.setFont(u8g2_font_7x14_mr);
  }
}

uint16_t centerPosition(const char * text) {
  return (tft.width() / 2) - (u8g.getUTF8Width(text) / 2);
}

void showRSSI_TEXTDisplay(bool firstrun) {
  static int last_rssi = 128;

  if (firstrun) {
    tft.fillRect(0, 0, tft.width(), tft.height(), ILI9341_BLACK);

    u8g.setFont(u8g2_font_helvB24_tf);
    u8g.setForegroundColor(ILI9341_WHITE);
    const char * headertext = "RSSI - Level";
    u8g.setCursor(centerPosition(headertext), 32);
    u8g.print(headertext);

    u8g.setFont(u8g2_font_helvB24_tr);
    tft.drawRoundRect(60, 70, 200, 100, 4, ILI9341_LIGHTGREY);
    u8g.setForegroundColor(ILI9341_GREEN);
    u8g.setCursor(170, 134);
    u8g.print("dBm");
    last_rssi = 1;
  }

  if (last_rssi != RSSILogTable[0].rssi) {
    uint8_t minus_width = u8g.getUTF8Width("-");

    u8g.setForegroundColor(ILI9341_BLACK);
    u8g.setCursor(96 - (last_rssi < -99 ? minus_width : 0), 134);
    u8g.print(last_rssi);

    u8g.setForegroundColor(ILI9341_GREEN);
    u8g.setCursor(96 - (RSSILogTable[0].rssi < -99 ? minus_width : 0) , 134);
    u8g.print(RSSILogTable[0].rssi);

    last_rssi = RSSILogTable[0].rssi;
  }
}

void showRSSI_GRAPHICDisplay(bool firstrun) {
  const int8_t rss_min = -110;
  const int8_t rss_max = -40;

  if (firstrun) {
    tft.fillRect(0, 0, tft.width(), tft.height(), ILI9341_BLACK);
    tft.drawLine(30, 1, 30, 240, ILI9341_WHITE);

    u8g.setFont(u8g2_font_6x10_tr);
    u8g.setForegroundColor(ILI9341_WHITE);
    u8g.setCursor(2, 226);
    u8g.print(rss_min);
    u8g.setCursor(8, 10);
    u8g.print(rss_max);

    for (uint8_t i = 0; i < 10; i++)
      tft.drawLine(26, i * 22 + 15, 30, i * 22 + 15, ILI9341_WHITE);

    tft.drawLine(0, 230, 320, 230, ILI9341_WHITE);
  }

  static uint32_t last_rssiLogLength = 0;
  static int last_rssi = 128;

  if (last_rssiLogLength != rssiLogLength) {
    for (uint8_t i = 0; i < 57; i++) {
      tft.fillRect(32 + (i * 6), 0, 5, 229, ILI9341_BLACK);

      int8_t rss = (i >= rssiLogLength) ? rss_min : RSSILogTable[i].rssi;
      uint8_t h = map(rss, rss_min, rss_max, 0, 229);

      tft.fillRect(32 + (i * 6), 229 - h, 5, h, ILI9341_GREEN);
    }

    u8g.setForegroundColor(ILI9341_BLACK);
    u8g.setCursor(2, 122);
    u8g.print(last_rssi);

    u8g.setForegroundColor(ILI9341_GREEN);
    u8g.setCursor(2, 122);
    u8g.print(RSSILogTable[0].rssi);

    last_rssi = RSSILogTable[0].rssi;
  }
  last_rssiLogLength = rssiLogLength;
}
#endif
#endif
