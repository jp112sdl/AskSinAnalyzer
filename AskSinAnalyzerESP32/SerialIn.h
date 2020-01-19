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
#define STRPOS_PAYLOAD_BEGIN  23

bool fillLogTable(const _SerialBuffer &sb, uint8_t b) {
  bool dataIsRSSIOnly = ((sb.Msg).length() == 3);

#ifdef VDEBUG
  if (!dataIsRSSIOnly) {
    DPRINTLN(F("# PROCESSING SERIAL DATA #"));
    DPRINT("I ");
    DPRINT(dataIsRSSIOnly ? "R" : "P");
    DPRINT(" #");
    DPRINT(String(b));
    DPRINT(": ");
    DPRINTLN(sb.Msg);
  }
#endif

  String rssiIn = (sb.Msg).substring(STRPOS_RSSI_BEGIN, STRPOS_LENGTH_BEGIN);
  int rssi = -1 * (strtol(&rssiIn[0], NULL, 16) & 0xFF);

  addRssiValueToRSSILogTable(rssi, sb.t, dataIsRSSIOnly ? RSSITYPE_NONE : RSSITYPE_HMRF);  

  if (dataIsRSSIOnly) {
    return false;
  }

  String lengthIn = (sb.Msg).substring(STRPOS_LENGTH_BEGIN, STRPOS_COUNT_BEGIN);
  uint8_t len = strtol(&lengthIn[0], NULL, 16) & 0xFF;

  String countIn = (sb.Msg).substring(STRPOS_COUNT_BEGIN, STRPOS_FLAGS_BEGIN);
  uint8_t cnt = strtol(&countIn[0], NULL, 16) & 0xFF;
  
  String flagsStr =  (sb.Msg).substring(STRPOS_FLAGS_BEGIN, STRPOS_TYPE_BEGIN);
  uint8_t flags = strtol(&flagsStr[0], NULL, 16) & 0xFF;

  String typStr =  (sb.Msg).substring(STRPOS_TYPE_BEGIN, STRPOS_FROM_BEGIN);
  uint8_t typ = strtol(&typStr[0], NULL, 16) & 0xFF;
  
  String fromStr = "";
  String toStr = "";
  if (ONLINE_MODE && RESOLVE_ADDRESS) {
    fromStr = getSerialFromIntAddress(hexToDec((sb.Msg).substring(STRPOS_FROM_BEGIN, STRPOS_TO_BEGIN)));
    toStr = getSerialFromIntAddress(hexToDec((sb.Msg).substring(STRPOS_TO_BEGIN, STRPOS_PAYLOAD_BEGIN)));
  }

  if (fromStr == "")  fromStr = "  " + (sb.Msg).substring(STRPOS_FROM_BEGIN, STRPOS_TO_BEGIN) + "  ";
  if (toStr == "")    toStr = "  " + (sb.Msg).substring(STRPOS_TO_BEGIN, STRPOS_PAYLOAD_BEGIN) + "  ";

  char fromAddress[SIZE_ADDRESS];
  (sb.Msg).substring(STRPOS_FROM_BEGIN, STRPOS_TO_BEGIN).toCharArray(fromAddress, SIZE_ADDRESS);
  char toAddress[SIZE_ADDRESS];
  (sb.Msg).substring(STRPOS_TO_BEGIN, STRPOS_PAYLOAD_BEGIN).toCharArray(toAddress, SIZE_ADDRESS);
  char msg[SIZE_MSG];
  (sb.Msg).substring(3).toCharArray(msg, SIZE_MSG);
  String msgStr = "";
  for (uint8_t i = 0; i< SIZE_MSG; i++) {
    if (msg[i] == 0) break;
    msgStr += msg[i];
    if (i % 2) msgStr += " ";
  }


  LogTable.shift();

  LogTable[0].lognumber = allCount;
  LogTable[0].time = sb.t;
  LogTable[0].rssi = rssi;
  memcpy(LogTable[0].fromSerial, fromStr.c_str(), SIZE_SERIAL);
  memcpy(LogTable[0].toSerial, toStr.c_str(), SIZE_SERIAL);
  memcpy(LogTable[0].fromAddress, fromAddress, SIZE_ADDRESS);
  memcpy(LogTable[0].toAddress, toAddress, SIZE_ADDRESS);
  LogTable[0].len = len;
  LogTable[0].cnt = cnt;
  LogTable[0].typ = typ;
  LogTable[0].flags = flags;
  memcpy(LogTable[0].msg, msgStr.c_str(), SIZE_MSG);

  if (flags == 0x00) RSSILogTable[0].type = RSSITYPE_HMIP; //alter RSSI value type to HMIP

  writeLogEntryToSD(LogTable[0]);
  writeLogEntryToWebSocket(LogTable[0]);
  writeSessionLogToSPIFFS(LogTable[0]);

  allCount++;

#ifdef VDEBUG
  DPRINTLN(F("\nAdded to LogTable: "));
  dumpLogTableEntry(LogTable[0]);
  DPRINT(F("######## PROCESSING ")); DDEC(allCount); DPRINTLN(F(" END ########\n"));
#endif
  return true;
}

#endif
