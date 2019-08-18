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
        SerialBuffer[msgBufferCount].t = now();
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

void fillLogTable(const _SerialBuffer &sb, uint8_t b) {
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


  shiftLogArray();

  LogTable[0].lognumber = allCount;
  LogTable[0].time = sb.t;
  LogTable[0].rssi = rssi;
  memcpy(LogTable[0].fromSerial, fromStr.c_str(), SIZE_SERIAL);
  memcpy(LogTable[0].toSerial, toStr.c_str(), SIZE_SERIAL);
  memcpy(LogTable[0].fromAddress, fromAddress, SIZE_ADDRESS);
  memcpy(LogTable[0].toAddress, toAddress, SIZE_ADDRESS);
  LogTable[0].len = len;
  LogTable[0].cnt = cnt;
  memcpy(LogTable[0].typ, typ.c_str(), SIZE_TYPE);
  memcpy(LogTable[0].flags, flags.c_str(), SIZE_FLAGS);

  writeLogEntryToCSV(LogTable[0]);
  writeLogEntryToWebSocket(LogTable[0]);
  writeSessionLogToSPIFFS(LogTable[0]);

  if (logLength < MAX_LOG_ENTRIES - 1) logLength++;

  DPRINTLN(F("\nAdded to LogTable: "));
  dumpLogTableEntry(LogTable[0]);
  //DPRINTLN(" => messages received: " + String(allCount));
  //DPRINTLN("logLength        = " + String(logLength));
  //DPRINTLN("logLengthDisplay = " + String(logLengthDisplay));

  allCount++;

  DPRINT(F("######## PROCESSING ")); DDEC(allCount); DPRINTLN(F(" END ########\n"));
}

#endif
