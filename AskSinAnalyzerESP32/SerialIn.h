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

// substring: ending index is exclusive, so it is fine to use the next starting index as end
#define STRPOS_RSSI_BEGIN     1
#define STRPOS_LENGTH_BEGIN   3
#define STRPOS_COUNT_BEGIN    5
#define STRPOS_FLAGS_BEGIN    7
#define STRPOS_TYPE_BEGIN     9
#define STRPOS_FROM_BEGIN     11
#define STRPOS_TO_BEGIN       17
#define STRPOS_TO_END         23

void fillLogTable(struct _SerialBuffer sb, uint8_t b) {
  DPRINTLN(F("######## PROCESSING NEW MESSAGE ########"));
  DPRINTLN("I #" + String(b) + ": " + sb.Msg);

  String rssiIn = (sb.Msg).substring(STRPOS_RSSI_BEGIN, STRPOS_LENGTH_BEGIN);
  int rssi = -1 * (strtol(&rssiIn[0], NULL, 16) & 0xFF);

  String lengthIn = (sb.Msg).substring(STRPOS_LENGTH_BEGIN, STRPOS_COUNT_BEGIN);
  uint8_t len = (strtol(&lengthIn[0], NULL, 16) & 0xFF);

  String countIn = (sb.Msg).substring(STRPOS_COUNT_BEGIN, STRPOS_FLAGS_BEGIN);
  uint8_t cnt = (strtol(&countIn[0], NULL, 16) & 0xFF);

  String flags = getFlags((sb.Msg).substring(STRPOS_FLAGS_BEGIN, STRPOS_TYPE_BEGIN));
  String typ = getTyp((sb.Msg).substring(STRPOS_TYPE_BEGIN, STRPOS_FROM_BEGIN));

  String fromStr = "";
  String toStr = "";
  if (ONLINE_MODE && RESOLVE_ADDRESS) {
    fromStr = getSerialFromIntAddress(hexToDec((sb.Msg).substring(STRPOS_FROM_BEGIN, STRPOS_TO_BEGIN)));
    toStr = getSerialFromIntAddress(hexToDec((sb.Msg).substring(STRPOS_TO_BEGIN, STRPOS_TO_END)));
  }
  
  if (fromStr == "")  fromStr = "  " + (sb.Msg).substring(STRPOS_FROM_BEGIN, STRPOS_TO_BEGIN) + "  ";
  if (toStr == "")    toStr = "  " + (sb.Msg).substring(STRPOS_TO_BEGIN, STRPOS_TO_END) + "  ";

  char fromAddress[SIZE_ADDRESS];
  (sb.Msg).substring(STRPOS_FROM_BEGIN, STRPOS_TO_BEGIN).toCharArray(fromAddress, SIZE_ADDRESS);
  char toAddress[SIZE_ADDRESS];
  (sb.Msg).substring(STRPOS_TO_BEGIN, STRPOS_TO_END).toCharArray(toAddress, SIZE_ADDRESS);


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
