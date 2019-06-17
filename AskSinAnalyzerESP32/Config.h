//- -----------------------------------------------------------------------------------------------------------------------
// AskSinAnalyzer
// 2019-06-01 jp112sdl Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------

#define JSONCONFIG_IP                     "ip"
#define JSONCONFIG_NETMASK                "netmask"
#define JSONCONFIG_GW                     "gw"
#define JSONCONFIG_NTP                    "ntp"
#define JSONCONFIG_CCUIP                  "ccuip"
#define JSONCONFIG_SVANALYZEINPUT         "svanalyzeinput"
#define JSONCONFIG_SVANALYZEOUTPUT        "svanalyzeoutput"

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
          DPRINTLN(F(" - JSON DeserializationError"));
          return false;
        } else {
          JsonObject json = doc.as<JsonObject>();
          DPRINTLN(F(" - Content of JSON Configfile:"));
          serializeJson(doc, Serial);
          DPRINTLN(F("\n - > JSON OK"));

          ((json[JSONCONFIG_IP]).as<String>()).toCharArray(NetConfig.ip, IPSIZE);
          ((json[JSONCONFIG_NETMASK]).as<String>()).toCharArray(NetConfig.netmask, IPSIZE);
          ((json[JSONCONFIG_GW]).as<String>()).toCharArray(NetConfig.gw, IPSIZE);

          String _ntp = (json[JSONCONFIG_NTP]).as<String>();
          strcpy(NetConfig.ntp, (_ntp == "null") ? DEFAULT_NTP_SERVER : _ntp.c_str());

          ((json[JSONCONFIG_CCUIP]).as<String>()).toCharArray(HomeMaticConfig.ccuIP, IPSIZE);
          ((json[JSONCONFIG_SVANALYZEINPUT]).as<String>()).toCharArray(HomeMaticConfig.SVAnalyzeInput, VARIABLESIZE);
          ((json[JSONCONFIG_SVANALYZEOUTPUT]).as<String>()).toCharArray(HomeMaticConfig.SVAnalyzeOutput, VARIABLESIZE);
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
  json[JSONCONFIG_NTP] = NetConfig.ntp;
  json[JSONCONFIG_CCUIP] = HomeMaticConfig.ccuIP;
  json[JSONCONFIG_SVANALYZEINPUT] = HomeMaticConfig.SVAnalyzeInput;
  json[JSONCONFIG_SVANALYZEOUTPUT] = HomeMaticConfig.SVAnalyzeOutput;

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

