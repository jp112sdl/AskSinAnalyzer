//- -----------------------------------------------------------------------------------------------------------------------
// AskSinAnalyzer
// 2019-06-01 jp112sdl Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------
#ifndef CONFIG_H_
#define CONFIG_H_

#define JSONCONFIG_IP                     "ip"
#define JSONCONFIG_NETMASK                "netmask"
#define JSONCONFIG_GW                     "gw"
#define JSONCONFIG_HOSTNAME               "hostname"
#define JSONCONFIG_NTP                    "ntp"
#define JSONCONFIG_CCUIP                  "ccuip"

bool loadSystemConfig() {
  DPRINTLN(F("- LOADING CONFIG"));
  if (spiffsAvailable == true) {
    DPRINTLN(F(" - mounted file system"));
    if (SPIFFS.exists(CONFIG_FILENAME)) {
      //file exists, reading and loading
      DPRINTLN(F(" - reading config file"));
      File configFile = SPIFFS.open(CONFIG_FILENAME, "r");
      if (configFile) {
        DPRINTLN(F(" - opened config file"));
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, buf.get());
        if (error) {
          DPRINT(F(" - JSON DeserializationError")); DPRINTLN(error.c_str());
          return false;
        } else {
          JsonObject json = doc.as<JsonObject>();
          DPRINTLN(F(" - Content of JSON Configfile:"));
          serializeJson(doc, Serial);
          DPRINTLN(F("\n - > JSON OK"));

          ((json[JSONCONFIG_IP]).as<String>()).toCharArray(NetConfig.ip, IPSIZE);
          ((json[JSONCONFIG_NETMASK]).as<String>()).toCharArray(NetConfig.netmask, IPSIZE);
          ((json[JSONCONFIG_GW]).as<String>()).toCharArray(NetConfig.gw, IPSIZE);

          String _hostname = (json[JSONCONFIG_HOSTNAME]).as<String>();
          strcpy(NetConfig.hostname, (_hostname == "null") ? DEFAULT_HOSTNAME : _hostname.c_str());

          String _ntp = (json[JSONCONFIG_NTP]).as<String>();
          strcpy(NetConfig.ntp, (_ntp == "null") ? DEFAULT_NTP_SERVER : _ntp.c_str());

          ((json[JSONCONFIG_CCUIP]).as<String>()).toCharArray(HomeMaticConfig.ccuIP, IPSIZE);
        }
      }
    } else {
      DPRINT(F(" - CONFIG File")); DPRINT(CONFIG_FILENAME); DPRINTLN(F(" does not exist."));
      return false;
    }
  } else {
    DPRINTLN(F(" - SPIFFS not available."));
    return false;
  }
  DPRINTLN(F("- LOADING CONFIG: SUCCESSFUL"));
  return true;
}

bool saveSystemConfig() {
  DPRINTLN(F(" - saveSystemConfig(): saving config"));
  StaticJsonDocument<1024> doc;
  JsonObject json = doc.to<JsonObject>();

  json[JSONCONFIG_IP] = NetConfig.ip;
  json[JSONCONFIG_NETMASK] = NetConfig.netmask;
  json[JSONCONFIG_GW] = NetConfig.gw;
  json[JSONCONFIG_HOSTNAME] = NetConfig.hostname;
  json[JSONCONFIG_NTP] = NetConfig.ntp;
  json[JSONCONFIG_CCUIP] = HomeMaticConfig.ccuIP;

  File configFile = SPIFFS.open(CONFIG_FILENAME, "w");
  if (!configFile) {
    DPRINTLN(F(" - saveSystemConfig(): failed to open config file for writing"));
    return false;
  }
  serializeJson(doc, Serial);
  DPRINTLN(F(""));
  serializeJson(doc, configFile);
  configFile.close();
  return true;
}

bool bootConfigMode() {
  Preferences configPreferences;
  configPreferences.begin("config", false);
  bool b = configPreferences.getBool("bootConfigMode", false);
  configPreferences.end();
  return b;
}

void bootConfigMode(bool b) {
  Preferences configPreferences;
  configPreferences.begin("config", false);
  configPreferences.putBool("bootConfigMode", b);
  delay(300);
  configPreferences.end();
}

#endif
