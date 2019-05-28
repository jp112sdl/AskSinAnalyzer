#ifndef DISPLAY_H_
#define DISPLAY_H_

void drawStatusCircle(uint16_t color) {
  if (isOnline)
    tft.fillCircle(5, 5, 5, color);
  else
    tft.drawCircle(5, 5, 5, color);
}

void initTFT() {
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
}

void refreshDisplayLog() {
  for (uint8_t c = 0; c <= logLengthDisplay; c++) {
    u8g.setCursor(0, DISPLAY_LOG_OFFSET_TOP + (DISPLAY_LOG_LINE_HEIGHT * LOG_BLOCK_SIZE * c));
    u8g.setFont(u8g2_font_9x15_mr);
    u8g.setForegroundColor(ILI9341_WHITE);
    u8g.print(LogTable[c].from);
    u8g.print(" ");
    u8g.print(LogTable[c].to);
    u8g.print(" ");

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

  if (logLengthDisplay < DISPLAY_LOG_LINES - 1) logLengthDisplay++;
}

#endif
