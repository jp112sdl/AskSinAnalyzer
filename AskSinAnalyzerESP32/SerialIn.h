//- -----------------------------------------------------------------------------------------------------------------------
// AskSinAnalyzer
// 2019-06-01 jp112sdl Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------

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
      //DPRINTLN("MESSAGE #"+String(msgBufferCount)+" ADDED: "+inStr);
      msgBuffer[msgBufferCount] = inStr;
      inStr = "";
      msgBufferCount++;
      if (msgBufferCount > 1) {
        DPRINTLN(F("****************"));
        DPRINT(F("!message Buffer = "));
        DPRINTLN(String(msgBufferCount));
        DPRINTLN(F("****************"));
      }
      allCount++;
    }
  }
}

void fillLogTable(String in, uint8_t b) {
  DPRINTLN(F("######## PROCESSING NEW MESSAGE ########"));
  DPRINTLN("I #" + String(b) + ": " + in);

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
  DPRINTLN(F("Preprocessing CSV"));
  String csvLine = "";
  String temp = "";
  csvLine += String(allCount);
  csvLine += ";";
  csvLine += getDatum(LogTable[0].time) + " " + getUhrzeit(LogTable[0].time);
  csvLine += ";";
  csvLine += String(LogTable[0].rssi);
  csvLine += ";";
  temp = LogTable[0].from;
  temp.trim();
  csvLine += temp;
  csvLine += ";";
  temp = LogTable[0].to;
  temp.trim();
  csvLine += temp;
  csvLine += ";";
  csvLine += String(LogTable[0].len);
  csvLine += ";";
  csvLine += String(LogTable[0].cnt);
  csvLine += ";";
  temp = LogTable[0].typ;
  temp.trim();
  csvLine += temp;
  csvLine += ";";
  temp = LogTable[0].flags;
  temp.trim();
  csvLine += temp;
  csvLine += ";";

  if (SPIFFS.totalBytes() - SPIFFS.usedBytes() > csvLine.length())
    writeCSV(CSV_FILENAME, csvLine);
  else
    DPRINTLN(F("writeCSV failed - not enough space"));

  if (logLength < MAX_LOG_ENTRIES - 1) logLength++;

  DPRINTLN(F("\nAdded to LogTable: "));
  DPRINT(F(" - from : ")); DPRINTLN(LogTable[0].from);
  DPRINT(F(" - to   : ")); DPRINTLN(LogTable[0].to);
  DPRINT(F(" - rssi : ")); DPRINTLN(LogTable[0].rssi);
  DPRINT(F(" - len  : ")); DPRINTLN(LogTable[0].len);
  DPRINT(F(" - cnt  : ")); DPRINTLN(LogTable[0].cnt);
  DPRINT(F(" - typ  : ")); DPRINTLN(LogTable[0].typ);
  DPRINT(F(" - flags: ")); DPRINTLN(LogTable[0].flags);
  DPRINT(F(" - time : ")); DPRINTLN(getDatum(LogTable[0].time) + " " + getUhrzeit(LogTable[0].time));
  //DPRINTLN(" => messages received: " + String(allCount));
  //DPRINTLN("logLength        = " + String(logLength));
  //DPRINTLN("logLengthDisplay = " + String(logLengthDisplay));
  DPRINT(F("######## PROCESSING ")); DDEC(allCount); DPRINTLN(F(" END ########\n"));
}

#endif
