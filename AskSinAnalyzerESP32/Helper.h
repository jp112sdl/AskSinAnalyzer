//- -----------------------------------------------------------------------------------------------------------------------
// AskSinAnalyzer
// 2019-06-01 jp112sdl Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------

#ifndef HELPER_H_
#define HELPER_H_

bool isNotEmpty(const char *string) {
  return *string;
}

void parseBytes(const char* str, char sep, byte* bytes, int maxBytes, int base) {
  for (int i = 0; i < maxBytes; i++) {
    bytes[i] = strtoul(str, NULL, base);
    str = strchr(str, sep);
    if (str == NULL || *str == '\0') {
      break;
    }
    str++;
  }
}

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
  else if (in == "83" || in == "8E") typ += "HMIP_TYPE";

  else typ = in;
  uint8_t typlen = typ.length();
  if (typ.length() < 30)
    for (uint8_t i = 0; i < (30 - typlen); i++)
      typ += " ";

  return typ;
}


void initAddressTable() {
  memset(AddressTable, 0, ADDRESSTABLE_LENGTH);
  /*for (uint16_t c = 0; c < ADDRESSTABLE_LENGTH; c++) {
    AddressTable[c].Address = "";
    AddressTable[c].Serial = "";
    }*/
}

void initLogTable() {
  memset(LogTable, 0, MAX_LOG_ENTRIES);
  /*for (uint16_t c = 0; c < MAX_LOG_ENTRIES; c++) {
    memset(LogTable[c].from, 0, 11);
    memset(LogTable[c].to, 0, 11);
    memset(LogTable[c].typ, 0, 32);
    memset(LogTable[c].flags, 0, 32);
    }*/
}

unsigned int hexToDec(String hexString) {
  unsigned int decValue = 0;
  int nextInt;

  for (int i = 0; i < hexString.length(); i++) {

    nextInt = int(hexString.charAt(i));
    if (nextInt >= 48 && nextInt <= 57) nextInt = map(nextInt, 48, 57, 0, 9);
    if (nextInt >= 65 && nextInt <= 70) nextInt = map(nextInt, 65, 70, 10, 15);
    if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
    nextInt = constrain(nextInt, 0, 15);

    decValue = (decValue * 16) + nextInt;
  }
  return decValue;
}

void createJSONDevList() {
  if (isOnline) {
    const size_t listCapacity = JSON_ARRAY_SIZE(400) + JSON_OBJECT_SIZE(2) + 400 * JSON_OBJECT_SIZE(3) + 4 * 4620;
    DynamicJsonDocument JSONDevList(listCapacity);

    String a = getCCURequest("AskSinAnalyzerDevList");
    a.replace("&quot;", "\"");

    DeserializationError error = deserializeJson(JSONDevList, a);
    if (error) {
      DPRINTLN(F(" - JSON DeserializationError"));
    } else {
      devices = JSONDevList["devices"];
      //for (uint16_t i = 0; i < devices.size(); i++) {
      //  JsonObject device = devices[i];
      //  DPRINTLN("(" + String(device["address"].as<unsigned int>()) + ") - " + device["serial"].as<String>() + " - " + device["name"].as<String>());
      //}
    }
  } else {
    DPRINTLN(F("- ABORTED. Not online."));
  }
}

String getSerialFromIntAddress(int intAddr) {
  if (isOnline) {
    if (devices.size() > 1) {
      for (uint16_t i = 0; i < devices.size(); i++) {
        JsonObject device = devices[i];
        if (device["address"].as<unsigned int>() == intAddr) {
          String _t =  device["serial"].as<String>();
          _t += "         ";
          return _t.substring(0, 10);
        }
      }
    }
  }
  return "";
}

#endif
