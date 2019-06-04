#ifndef SERIALIN_H_
#define SERIALIN_H_

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
      if (msgBufferCount > 1) {
        Serial.println(F("****************"));
        Serial.print(F("!message Buffer = "));
        Serial.println(String(msgBufferCount));
        Serial.println(F("****************"));
      }
      allCount++;
    }
  }
}

void fillLogTable(String in, uint8_t b) {
  Serial.println("I #" + String(b) + ": " + in);

  String rssiIn = in.substring(1, 3);
  int rssi = -1 * (strtol(&rssiIn[0], NULL, 16) & 0xFF);

  String lengthIn = in.substring(3, 5);
  uint8_t len = (strtol(&lengthIn[0], NULL, 16) & 0xFF);

  String countIn = in.substring(5, 7);
  uint8_t cnt = (strtol(&countIn[0], NULL, 16) & 0xFF);

  String flags = getFlags(in.substring(7, 9));
  String typ = getTyp(in.substring(9, 11));

  String fromStr = "";
  String toStr = "";
  if (ONLINE_MODE && RESOLVE_ADDRESS) {
    fromStr = getSerialFromAddress(in.substring(11, 17));
    toStr = getSerialFromAddress(in.substring(17, 23));
  } else {
    fromStr = "  " + in.substring(11, 17) + "  ";
    toStr = "  " + in.substring(17, 23) + "  ";
  }

  if (logLength > 0) {
    for (uint16_t c = logLength; c > 0; c--) {
      memcpy(LogTable[c].from, LogTable[c - 1].from, 10);
      memcpy(LogTable[c].to, LogTable[c - 1].to, 10);
      LogTable[c].rssi = LogTable[c - 1].rssi;
      LogTable[c].len = LogTable[c - 1].len;
      LogTable[c].cnt = LogTable[c - 1].cnt;
      memcpy(LogTable[c].typ, LogTable[c - 1].typ, 30);
      memcpy(LogTable[c].flags, LogTable[c - 1].flags, 30);
      LogTable[c].time = LogTable[c - 1].time;
      LogTable[c].lognumber = LogTable[c - 1].lognumber;
    }
  }

  LogTable[0].lognumber = allCount;
  LogTable[0].time = ((timeOK == true)  ? now() : 0);
  LogTable[0].rssi = rssi;
  memcpy(LogTable[0].from, fromStr.c_str(), 10);
  memcpy(LogTable[0].to, toStr.c_str(), 10);
  LogTable[0].len = len;
  LogTable[0].cnt = cnt;
  memcpy(LogTable[0].typ, typ.c_str(), 30);
  memcpy(LogTable[0].flags, flags.c_str(), 30);

  // Write to CSV
  String csvLine = "";
  String temp = "";
  csvLine += String(allCount);
  csvLine += "; ";
  csvLine += getDatum(LogTable[0].time) + " " + getUhrzeit(LogTable[0].time);
  csvLine += "; ";
  csvLine += String(LogTable[0].rssi);
  csvLine += "; ";
  temp = LogTable[0].from;
  temp.trim();
  csvLine += temp;
  csvLine += "; ";
  temp = LogTable[0].to;
  temp.trim();
  csvLine += temp;
  csvLine += "; ";
  csvLine += String(LogTable[0].len);
  csvLine += "; ";
  csvLine += String(LogTable[0].cnt);
  csvLine += "; ";
  temp = LogTable[0].typ;
  temp.trim();
  csvLine += temp;
  csvLine += "; ";
  temp = LogTable[0].flags;
  temp.trim();
  csvLine += temp;
  csvLine += "; ";

  if (SPIFFS.totalBytes() - SPIFFS.usedBytes() > csvLine.length())
    writeCSV(CSV_FILENAME, csvLine);
  else
    Serial.println(F("writeCSV failed - not enough space"));
  //

  if (logLength < MAX_LOG_ENTRIES - 1) logLength++;

  Serial.println(F("Added to LogTable: "));
  Serial.print(F("  - from : ")); Serial.println(LogTable[0].from);
  Serial.print(F("  - to   : ")); Serial.println(LogTable[0].to);
  Serial.print(F("  - rssi : ")); Serial.println(LogTable[0].rssi);
  Serial.print(F("  - len  : ")); Serial.println(LogTable[0].len);
  Serial.print(F("  - cnt  : ")); Serial.println(LogTable[0].cnt);
  Serial.print(F("  - typ  : ")); Serial.println(LogTable[0].typ);
  Serial.print(F("  - flags: ")); Serial.println(LogTable[0].flags);
  Serial.print(F("  - time : ")); Serial.println(getDatum(LogTable[0].time) + " " + getUhrzeit(LogTable[0].time));
  Serial.println("allCount         = " + String(allCount));
  Serial.println("logLength        = " + String(logLength));
  Serial.println("logLengthDisplay = " + String(logLengthDisplay));
  Serial.println();

}

#endif
