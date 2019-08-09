//- -----------------------------------------------------------------------------------------------------------------------
// AskSinAnalyzer
// 2019-06-01 jp112sdl Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------

#ifndef SERIALIN_H_
#define SERIALIN_H_

void receiveMessages() {
  static String inStr = "";
  while (Serial1.available() > 0) {
    char inChar = (char)Serial1.read();
    if (inChar != ';') {
      if (inChar != '\n' && inChar != '\r') {
        inStr += inChar;
      }
    } else {
      bool messageFound = false;
      //DPRINTLN("MESSAGE #"+String(msgBufferCount)+" ADDED: "+inStr);
      /* each telegramm shall start with ':' and end with ';' */
      if (inStr[0] == ':') {
        messageFound = true;
      } else {
        if (inStr.startsWith("Packet too big")) {
          DPRINT(F("INVALID MESSAGE (too big) DISCARDED: ")); DPRINTLN(inStr);
        } else {
          int startPos = inStr.lastIndexOf(':');
          if (startPos == -1) {
            DPRINT(F("INVALID MESSAGE (no ':' found) DISCARDED: ")); DPRINTLN(inStr);
          } else {
            DPRINT(F("MESSAGE DOES NOT START WITH ':' ")); DPRINTLN(inStr);
            inStr = inStr.substring(startPos);
            messageFound = true;
            DPRINT(F("CORRECTED MESSAGE: ")); DPRINTLN(inStr);
          }
        }
      }
      if (messageFound) {
        SerialBuffer[msgBufferCount].Msg = inStr;
        SerialBuffer[msgBufferCount].t = now();// ((timeOK == true)  ? now() : millis());
        msgBufferCount++;
        if (msgBufferCount > 1) {
          DPRINTLN(F("****************"));
          DPRINT(F("!message Buffer = "));
          DDECLN(msgBufferCount);
          DPRINTLN(F("****************"));
        }
      }
      inStr = "";
    }
  }
}

void fillLogTable(struct _SerialBuffer sb, uint8_t b) {
  DPRINTLN(F("######## PROCESSING NEW MESSAGE ########"));
  DPRINTLN("I #" + String(b) + ": " + sb.Msg);

  String rssiIn = (sb.Msg).substring(1, 3);
  int rssi = -1 * (strtol(&rssiIn[0], NULL, 16) & 0xFF);

  String lengthIn = (sb.Msg).substring(3, 5);
  uint8_t len = (strtol(&lengthIn[0], NULL, 16) & 0xFF);

  String countIn = (sb.Msg).substring(5, 7);
  uint8_t cnt = (strtol(&countIn[0], NULL, 16) & 0xFF);

  String flags = getFlags((sb.Msg).substring(7, 9));
  String typ = getTyp((sb.Msg).substring(9, 11));

  String fromStr = "";
  String toStr = "";
  if (ONLINE_MODE && RESOLVE_ADDRESS) {
    fromStr = getSerialFromAddress((sb.Msg).substring(11, 17));
    toStr = getSerialFromAddress((sb.Msg).substring(17, 23));
  } else {
    fromStr = "  " + (sb.Msg).substring(11, 17) + "  ";
    toStr = "  " + (sb.Msg).substring(17, 23) + "  ";
  }

  char fromAddress[SIZE_ADDRESS];
  (sb.Msg).substring(11, 17).toCharArray(fromAddress, SIZE_ADDRESS);
  char toAddress[SIZE_ADDRESS];
  (sb.Msg).substring(17, 23).toCharArray(toAddress, SIZE_ADDRESS);


  if (logLength > 0) {
    for (uint16_t c = logLength; c > 0; c--) {
      memcpy(LogTable[c].from, LogTable[c - 1].from, SIZE_SERIAL);
      memcpy(LogTable[c].to, LogTable[c - 1].to, SIZE_SERIAL);
      memcpy(LogTable[c].fromAddress, LogTable[c - 1].fromAddress, SIZE_ADDRESS);
      memcpy(LogTable[c].toAddress, LogTable[c - 1].toAddress, SIZE_ADDRESS);
      LogTable[c].rssi = LogTable[c - 1].rssi;
      LogTable[c].len = LogTable[c - 1].len;
      LogTable[c].cnt = LogTable[c - 1].cnt;
      memcpy(LogTable[c].typ, LogTable[c - 1].typ, SIZE_TYPE);
      memcpy(LogTable[c].flags, LogTable[c - 1].flags, SIZE_FLAGS);
      LogTable[c].time = LogTable[c - 1].time;
      LogTable[c].lognumber = LogTable[c - 1].lognumber;
    }
  }

  LogTable[0].lognumber = allCount;
  LogTable[0].time = sb.t;
  LogTable[0].rssi = rssi;
  memcpy(LogTable[0].from, fromStr.c_str(), SIZE_SERIAL);
  memcpy(LogTable[0].to, toStr.c_str(), SIZE_SERIAL);
  memcpy(LogTable[0].fromAddress, fromAddress, SIZE_ADDRESS);
  memcpy(LogTable[0].toAddress, toAddress, SIZE_ADDRESS);
  LogTable[0].len = len;
  LogTable[0].cnt = cnt;
  memcpy(LogTable[0].typ, typ.c_str(), SIZE_TYPE);
  memcpy(LogTable[0].flags, flags.c_str(), SIZE_FLAGS);

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

      
  temp = LogTable[0].fromAddress;
  temp.trim();
  csvLine += temp;
  csvLine += ";";
  
  temp = LogTable[0].from;
  temp.trim();
  csvLine += temp;
  csvLine += ";";

  temp = LogTable[0].toAddress;
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
  DPRINT(F(" - fromAddress : ")); DPRINTLN(LogTable[0].fromAddress);
  DPRINT(F(" - fromSerial  : ")); DPRINTLN(LogTable[0].from);
  DPRINT(F(" - toAddress   : ")); DPRINTLN(LogTable[0].toAddress);
  DPRINT(F(" - toSerial    : ")); DPRINTLN(LogTable[0].to);
  DPRINT(F(" - rssi        : ")); DPRINTLN(LogTable[0].rssi);
  DPRINT(F(" - len         : ")); DPRINTLN(LogTable[0].len);
  DPRINT(F(" - cnt         : ")); DPRINTLN(LogTable[0].cnt);
  DPRINT(F(" - typ         : ")); DPRINTLN(LogTable[0].typ);
  DPRINT(F(" - flags       : ")); DPRINTLN(LogTable[0].flags);
  DPRINT(F(" - time        : ")); DPRINTLN(getDatum(LogTable[0].time) + " " + getUhrzeit(LogTable[0].time));
  //DPRINTLN(" => messages received: " + String(allCount));
  //DPRINTLN("logLength        = " + String(logLength));
  //DPRINTLN("logLengthDisplay = " + String(logLengthDisplay));

  allCount++;

  DPRINT(F("######## PROCESSING ")); DDEC(allCount); DPRINTLN(F(" END ########\n"));
}

#endif
