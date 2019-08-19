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
  if (flagsInt == 0x00) flags = "HMIP_UNKNOWN";
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
  else if (in.startsWith("8")) typ = "HMIP_TYPE";

  else typ = in;
  uint8_t typlen = typ.length();
  if (typ.length() < 30)
    for (uint8_t i = 0; i < (30 - typlen); i++)
      typ += " ";

  return typ;
}

void initLogTable() {
  memset(LogTable, 0, MAX_LOG_ENTRIES);
}

String loadAskSinAnalyzerDevListFromCCU() {
  if (isOnline && WiFi.status() == WL_CONNECTED) {
    DPRINTLN(F("- Loading DevList from CCU... "));
#ifdef USE_DISPLAY
    drawStatusCircle(ILI9341_BLUE);
#endif
    HTTPClient http;
    //http.setTimeout(HTTPTimeOut);
    String url = "http://" + String(HomeMaticConfig.ccuIP) + ":8181/a.exe?ret=dom.GetObject(%22" + CCU_SV + "%22).Value()";
    //DPRINTLN("loadAskSinAnalyzerDevListFromCCU url: " + url);
    http.begin(url);
    int httpCode = http.GET();
    String payload = "ERROR";
    if (httpCode > 0) {
      payload = http.getString();
    }
    if (httpCode != 200) {
      DPRINT("HTTP failed with code "); DDECLN(httpCode);
    }
    http.end();

    payload = payload.substring(payload.indexOf("<ret>"));
    payload = payload.substring(5, payload.indexOf("</ret>"));
    payload.replace("&quot;", "\"");
    //DPRINTLN("result: " + payload);
#ifdef USE_DISPLAY
    drawStatusCircle(ILI9341_GREEN);
#endif
    return payload;
  }

  DPRINTLN(" - loadAskSinAnalyzerDevListFromCCU: ERROR");
#ifdef USE_DISPLAY
  drawStatusCircle(ILI9341_RED);
#endif
  return "ERROR";
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

const size_t listCapacity = JSON_ARRAY_SIZE(400) + JSON_OBJECT_SIZE(2) + 400 * JSON_OBJECT_SIZE(3) + 4 * 4620;
DynamicJsonDocument JSONDevList(listCapacity);
void createJSONDevList(String js) {
  DeserializationError error = deserializeJson(JSONDevList, js);
  if (error) {
    DPRINT(F(" - JSON DeserializationError: ")); DPRINTLN(error.c_str());
  } else {
    devices = JSONDevList["devices"];
    DPRINT(F(" - Device List created with ")); DDEC(devices.size()); DPRINTLN(F(" entries"));
    //for (uint16_t i = 0; i < devices.size(); i++) {
    //  JsonObject device = devices[i];
    //  DPRINTLN("(" + String(device["address"].as<unsigned int>()) + ") - " + device["serial"].as<String>() + " - " + device["name"].as<String>());
    //}
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

void shiftLogArray() {
  if (logLength > 0) {
    for (uint16_t c = logLength; c > 0; c--) {
      memcpy(LogTable[c].fromSerial, LogTable[c - 1].fromSerial, SIZE_SERIAL);
      memcpy(LogTable[c].toSerial, LogTable[c - 1].toSerial, SIZE_SERIAL);
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
}

String createCSVFromLogTableEntry(_LogTable lt, bool lng) {
  String csvLine = "";
  String temp = "";
  csvLine += String(lt.lognumber);
  csvLine += ";";
  csvLine += lng ? getDatum(lt.time) + " " + getUhrzeit(lt.time) : now();
  csvLine += ";";
  csvLine += String(lt.rssi);
  csvLine += ";";

  temp = lt.fromAddress;
  temp.trim();
  csvLine += temp;
  csvLine += ";";

  if (lng) {
    temp = lt.fromSerial;
    temp.trim();
    csvLine += temp;
    csvLine += ";";
  }

  temp = lt.toAddress;
  temp.trim();
  csvLine += temp;
  csvLine += ";";

  if (lng) {
    temp = lt.toSerial;
    temp.trim();
    csvLine += temp;
    csvLine += ";";
  }

  csvLine += String(lt.len);
  csvLine += ";";
  csvLine += String(lt.cnt);
  csvLine += ";";
  temp = lt.typ;
  temp.trim();
  csvLine += temp;
  csvLine += ";";
  temp = lt.flags;
  temp.trim();
  csvLine += temp;
  csvLine += ";";
  return csvLine;
}

String createJSONFromLogTableEntry(_LogTable &lt) {
  String json = "{";
  json += "\"lognumber\": " + String(lt.lognumber) + ", ";
  json += "\"tstamp\": " + String(lt.time) + ", ";
  json += "\"rssi\": " + String(lt.rssi) + ", ";
  String from = String(lt.fromAddress);
  from.trim();
  json += "\"from\": \"" + from + "\", ";
  String to = String(lt.toAddress);
  to.trim();
  json += "\"to\": \"" + to + "\", ";
  json += "\"len\": " + String(lt.len) + ", ";
  json += "\"cnt\": " + String(lt.cnt) + ", ";
  String t = String(lt.typ);
  t.trim();
  json += "\"typ\": \"" + t + "\", ";
  String fl = String(lt.flags);
  fl.trim();
  json += "\"flags\": \"" + fl + "\"";
  json += "}";
  return json;
}

void dumpLogTableEntry(_LogTable &lt) {
  DPRINT(F(" - fromAddress : ")); DPRINTLN(lt.fromAddress);
  DPRINT(F(" - fromSerial  : ")); DPRINTLN(lt.fromSerial);
  DPRINT(F(" - toAddress   : ")); DPRINTLN(lt.toAddress);
  DPRINT(F(" - toSerial    : ")); DPRINTLN(lt.toSerial);
  DPRINT(F(" - rssi        : ")); DPRINTLN(lt.rssi);
  DPRINT(F(" - len         : ")); DPRINTLN(lt.len);
  DPRINT(F(" - cnt         : ")); DPRINTLN(lt.cnt);
  DPRINT(F(" - typ         : ")); DPRINTLN(lt.typ);
  DPRINT(F(" - flags       : ")); DPRINTLN(lt.flags);
  DPRINT(F(" - time        : ")); DPRINTLN(getDatum(lt.time) + " " + getUhrzeit(lt.time));
}

#endif
