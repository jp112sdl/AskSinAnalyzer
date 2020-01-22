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
    u8g.setForegroundColor(getTyp(LogTable[c].typ).startsWith("HMIP") ? ILI9341_CYAN : ILI9341_WHITE);

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
      u8g.print(getTyp(LogTable[c].typ));
      u8g.setCursor(0, DISPLAY_LOG_OFFSET_TOP + (2 * DISPLAY_LOG_LINE_HEIGHT) + (DISPLAY_LOG_LINE_HEIGHT * LOG_BLOCK_SIZE * c));
      u8g.setForegroundColor(ILI9341_DARKGREY);
      u8g.print(" FLAGS: ");
      u8g.setForegroundColor(ILI9341_OLIVE);
      u8g.print(getFlags(LogTable[c].flags));
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

uint16_t getRssiForegroundColor(uint8_t type) {
  uint16_t fgColor = ILI9341_YELLOW;
  if (type == RSSITYPE_HMRF) fgColor = ILI9341_GREEN;
  else if (type == RSSITYPE_HMIP) fgColor = ILI9341_CYAN;
  return fgColor;
}

void showRSSI_TEXTDisplay(bool firstrun) {
  const uint8_t bar_height             = 220;
  const uint8_t bar_start_x            = 34;
  const uint8_t bar_width              = 20;
  const int8_t  rssi_min               = -120;
  const int8_t  rssi_max               =  -20;
  static int8_t last_rssi              = 128;
  static int8_t peak                   = -128;
  static int8_t last_peak              = 0;
  static unsigned long peak_set_millis = 0;
  static uint16_t peakFgCol            = ILI9341_BLACK;


  if (firstrun) {
    last_rssi = 128;
    peak = -128;
    last_peak = 0;
    peak_set_millis = millis();

    tft.fillRect(0, 0, tft.width(), tft.height(), ILI9341_BLACK);

    u8g.setFont(u8g2_font_helvB24_tf);
    u8g.setForegroundColor(ILI9341_WHITE);
    u8g.setCursor(102, 32);
    u8g.print("RSSI - Level");

    u8g.setFont(u8g2_font_helvB08_tr);
    u8g.setForegroundColor(ILI9341_WHITE);
    u8g.setCursor(2, 230);
    u8g.print(rssi_min);
    u8g.setCursor(8, 20);
    u8g.print(rssi_max);

    u8g.setFont(u8g2_font_helvB24_tr);
    tft.drawRoundRect(95, 70, 200, 100, 4, ILI9341_LIGHTGREY);
    tft.drawRoundRect(30, 6, 28, 228, 4, ILI9341_LIGHTGREY);
    u8g.setForegroundColor(ILI9341_WHITE);
    u8g.setCursor(190, 134);
    u8g.print("dBm");

    //print Peak-Source label
    u8g.setFont(u8g2_font_helvB14_tf);
    u8g.setForegroundColor(ILI9341_WHITE);
    const char * peak_source_caption = "Peak Source:";
    u8g.setCursor(95 + (100 - (u8g.getUTF8Width(peak_source_caption) / 2)), 200);
    u8g.print(peak_source_caption);

    last_rssi = 1;
  }

  if (last_rssi != RSSILogTable[0].rssi) {
    u8g.setFont(u8g2_font_helvB24_tr);

    uint8_t minus_width = u8g.getUTF8Width("-");

    int8_t rssi = RSSILogTable[0].rssi;
    if (rssi < rssi_min) rssi = rssi_min;
    if (rssi > rssi_max) rssi = rssi_max;

    uint16_t fgCol = getRssiForegroundColor(RSSILogTable[0].type);

    u8g.setForegroundColor(ILI9341_BLACK);
    u8g.setCursor(131 - (last_rssi < -99 ? minus_width : 0), 134);
    u8g.print(last_rssi);

    u8g.setForegroundColor(fgCol);
    u8g.setCursor(131 - (RSSILogTable[0].rssi < -99 ? minus_width : 0) , 134);
    u8g.print(RSSILogTable[0].rssi);

    uint8_t h = map(rssi, rssi_min, rssi_max, 0, bar_height);
    tft.fillRect(bar_start_x, 10, bar_width, bar_height, ILI9341_BLACK);
    tft.fillRect(bar_start_x, bar_height - h + 4, bar_width, h, fgCol);

    bool rssiPeakHoldNoiseFloorOnly =  digitalRead(RSSI_PEAK_HOLD_MODE_PIN) == LOW;

    if (rssiPeakHoldNoiseFloorOnly) {
      if (RSSILogTable[0].type == RSSITYPE_NONE) {
        peak = _max(peak, RSSILogTable[0].rssi);
      }
    } else {
      peak = _max(peak, RSSILogTable[0].rssi);
    }

    uint8_t peak_h = map(peak, rssi_min, rssi_max, 0, bar_height);

    if (last_peak != peak) {
      //print rssi peak cause

      u8g.setFont(u8g2_font_helvB14_tf);

      peakFgCol = getRssiForegroundColor(RSSILogTable[0].type);
      u8g.setForegroundColor(peakFgCol);

      uint8_t peak_source_left = 95 + (100 - (u8g.getUTF8Width(RSSILogTable[0].fromSerial) / 2));
      u8g.setCursor(peak_source_left, 220);
      tft.fillRect(95, 200, tft.width() - 95, 25, ILI9341_BLACK);
      u8g.print(RSSILogTable[0].fromSerial);

      //print peak value right to the bar
      tft.fillRect(60, 10, 30, bar_height, ILI9341_BLACK);
      u8g.setFont(u8g2_font_helvB08_tr);
      u8g.setForegroundColor(peakFgCol);
      u8g.setCursor(60, bar_height - peak_h + 8);
      u8g.print(peak);
    }

    if (peak > last_peak) peak_set_millis = millis();

    if (peak > rssi_min) tft.drawLine(bar_start_x, bar_height - peak_h + 4, bar_start_x + bar_width - 1, bar_height - peak_h + 4, peakFgCol);

    if (millis() - peak_set_millis > RSSI_PEAK_HOLD_MILLIS) {
      peak = -128;
      peak_set_millis = millis();
    }

    last_rssi = RSSILogTable[0].rssi;
    last_peak = peak;
  }
}

void showRSSI_GRAPHICDisplay(bool firstrun) {
  const uint8_t bar_height        = 229;
  const uint8_t bar_start_x       = 32;
  const int8_t  rssi_min          = -120;
  const int8_t  rssi_max          = -20;
  static bool last_rssiValueAdded = true;
  static int8_t last_rssi         = 128;

  uint8_t bar_width = RSSIConfig.histogramBarWidth;
  static uint8_t last_bar_width = 0;

  if (last_bar_width != bar_width) firstrun = true;

  if (firstrun) {
    tft.fillRect(0, 0, tft.width(), tft.height(), ILI9341_BLACK);
    tft.drawLine(30, 1, 30, 240, ILI9341_WHITE);

    u8g.setFont(u8g2_font_6x10_tr);
    u8g.setForegroundColor(ILI9341_WHITE);
    u8g.setCursor(2, 226);
    u8g.print(rssi_min);
    u8g.setCursor(8, 10);
    u8g.print(rssi_max);

    for (uint8_t i = 0; i < 10; i++)
      tft.drawLine(26, i * 22 + 15, 30, i * 22 + 15, ILI9341_WHITE);

    tft.drawLine(0, 230, 320, 230, ILI9341_WHITE);
  }

  if (last_rssiValueAdded != rssiValueAdded) {
    for (uint8_t i = 0; i < (tft.width() - bar_start_x)  / (bar_width + 1); i++) {
      int8_t rssi = RSSILogTable[i].rssi;
      if (i >= RSSILogTable.count() || rssi < rssi_min) rssi = rssi_min;
      if (rssi > rssi_max) rssi = rssi_max;

      uint8_t h = map(rssi, rssi_min, rssi_max, 0, bar_height);

      tft.fillRect(bar_start_x + (i * (bar_width + 1)), 0, bar_width, bar_height, ILI9341_BLACK);
      tft.fillRect(bar_start_x + (i * (bar_width + 1)), bar_height - h, bar_width, h, getRssiForegroundColor(RSSILogTable[i].type));
    }

    u8g.setForegroundColor(ILI9341_BLACK);
    u8g.setCursor(2, 118);
    u8g.print(last_rssi);

    u8g.setForegroundColor(getRssiForegroundColor(RSSILogTable[0].type));
    u8g.setCursor(2, 118);
    u8g.print(RSSILogTable[0].rssi);

    last_rssi = RSSILogTable[0].rssi;
  }
  last_rssiValueAdded = rssiValueAdded;
  last_bar_width = bar_width;
}

void showAPModeDisplay() {
  tft.fillRect(0, 0, tft.width(), tft.height(), ILI9341_BLACK);
  u8g.setFont(u8g2_font_helvB24_tf);
  u8g.setForegroundColor(ILI9341_RED);
  u8g.setCursor(80, 132);
  u8g.print("AP-Modus");
  u8g.setForegroundColor(ILI9341_WHITE);
}
#endif
#endif
