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
      allCount++;
    }
  }
}

void fillLogTable(String in) {
  Serial.println("I " + in);

  String rssiIn = in.substring(1, 3);
  int rssi = -1 * (strtol(&rssiIn[0], NULL, 16) & 0xFF);

  String lengthIn = in.substring(3, 5);
  uint8_t len = (strtol(&lengthIn[0], NULL, 16) & 0xFF);

  String countIn = in.substring(5, 7);
  uint8_t cnt = (strtol(&countIn[0], NULL, 16) & 0xFF);

  String flags = getFlags(in.substring(7, 9));
  String typ = getTyp(in.substring(9, 11));

#ifdef ONLINE_MODE
  String fromStr = getSerialFromAddress(in.substring(11, 17));
  String toStr = getSerialFromAddress(in.substring(17, 23));
#else
  String fromStr = "  " + in.substring(11, 17) + "  ";
  String toStr = "  " + in.substring(17, 23) + "  ";
#endif

  if (logLength > 0) {
    for (uint16_t c = logLength; c > 0; c--) {
      memcpy(LogTable[c].from, LogTable[c - 1].from, 10);
      memcpy(LogTable[c].to, LogTable[c - 1].to, 10);
      LogTable[c].rssi = LogTable[c - 1].rssi;
      LogTable[c].len = LogTable[c - 1].len;
      LogTable[c].cnt = LogTable[c - 1].cnt;
      memcpy(LogTable[c].typ, LogTable[c - 1].typ, 30);
      memcpy(LogTable[c].flags, LogTable[c - 1].flags, 30);
    }
  }

  memcpy(LogTable[0].from, fromStr.c_str(), 10);
  memcpy(LogTable[0].to, toStr.c_str(), 10);
  LogTable[0].rssi = rssi;
  LogTable[0].len = len;
  LogTable[0].cnt = cnt;
  memcpy(LogTable[0].typ, typ.c_str(), 30);
  memcpy(LogTable[0].flags, flags.c_str(), 30);
  if (logLength < MAX_LOG_ENTRIES - 1) logLength++;
}

#endif
