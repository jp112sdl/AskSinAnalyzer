#ifndef HELPER_H_
#define HELPER_H_

String getFlags(String in) {
  int flagsInt =  (strtol(&in[0], NULL, 16) & 0xFF);
  String flags = "";
  if (flagsInt & 0x01) flags += "WKUP ";
  if (flagsInt & 0x02) flags += "WKMEUP ";
  if (flagsInt & 0x04) flags += "BCAST ";
  if (flagsInt & 0x10) flags += "BURST ";
  if (flagsInt & 0x20) flags += "BIDI ";
  if (flagsInt & 0x40) flags += "RPTED ";
  if (flagsInt & 0x80) flags += "RPTEN ";
  uint8_t flagslen = flags.length();
  if (flags.length() < 30)
    for (uint8_t i = 0; i < (30 - flagslen); i++)
      flags += " ";

  return flags;
}

String getTyp(String in) {
  String typ = "";
  if (in == "00") typ = "DEVINFO";
  else if (in == "01") typ = "CONFIG";
  else if (in == "02") typ = "RESPONSE";
  else if (in == "03") typ = "RESPONSE_AES";
  else if (in == "04") typ = "KEY_EXCHANGE";
  else if (in == "10") typ = "INFO";
  else if (in == "11") typ = "ACTION";
  else if (in == "12") typ = "HAVE_DATA";
  else if (in == "3E") typ = "SWITCH_EVENT";
  else if (in == "3F") typ = "TIMESTAMP";
  else if (in == "40") typ = "REMOTE_EVENT";
  else if (in == "41") typ = "SENSOR_EVENT";
  else if (in == "53") typ = "SENSOR_DATA";
  else if (in == "58") typ = "CLIMATE_EVENT";
  else if (in == "5A") typ = "CLIMATECTRL_EVENT";
  else if (in == "5E") typ = "POWER_EVENT";
  else if (in == "5F") typ = "POWER_EVENT_CYCLIC";
  else if (in == "70") typ = "WEATHER";
  else typ = in;
  uint8_t typlen = typ.length();
  if (typ.length() < 30)
    for (uint8_t i = 0; i < (30 - typlen); i++)
      typ += " ";

  return typ;
}

String getSerialFromAddress(String in) {
  String out = in;

  if (in != "000000") {
    for (uint16_t c = 0; c < ADDRESSTABLE_LENGTH; c++) {
      if (AddressTable[c].Address == in) {
        String s = (AddressTable[c].Serial).substring(0, 10);
        Serial.println("FOUND LOCAL: " + in + " / " + s);
        drawStatusCircle(ILI9341_GREEN);
        return s;
      }
    }

    if  (WiFi.status() == WL_CONNECTED) {
      if (setCCURequest(INPUT_SV, in) == true) {
        drawStatusCircle(ILI9341_GREEN);
        delay(250);
        String res = getCCURequestResult(OUTPUT_SV);
        if (res.length() > 7 && res.substring(0, 6) == in) {
          drawStatusCircle(ILI9341_GREEN);
          Serial.println("FOUND VALID RESULT");
          AddressTable[AddressTableCount].Address = in;
          res = res.substring(res.indexOf(","));
          res = res.substring(1);
          res.replace("BidCoS-RF", "-ZENTRALE-");

          Serial.println("SERIAL = " + res);
          AddressTable[AddressTableCount].Serial = res;
          out = res;
          AddressTableCount++;
        } else {
          drawStatusCircle(ILI9341_RED);
        }
      } else {
        drawStatusCircle(ILI9341_RED);
      }
    }
  } else {
    out = "-ALLE-";
  }
  out.trim();
  if (out.length() == 6) out = "  " + out + "  ";
  return out;
}

void initLogTable() {
  for (uint16_t c = 0; c < MAX_LOG_ENTRIES; c++) {
    memset(LogTable[c].from, 0, 11);
    memset(LogTable[c].to, 0, 11);
    memset(LogTable[c].typ, 0, 32);
    memset(LogTable[c].flags, 0, 32);
  }
}

#endif
