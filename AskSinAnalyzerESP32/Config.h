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

void dumpSystemConfig() {
  DPRINTLN(F("- dump config:"));
  DPRINT(F(" - Hostname    : ")); DPRINTLN(NetConfig.hostname);
  DPRINT(F(" - CCU IP      : ")); DPRINTLN(HomeMaticConfig.ccuIP);
  DPRINT(F(" - NTP         : ")); DPRINTLN(NetConfig.ntp);
  DPRINT(F(" - Static IP   : ")); DPRINTLN(NetConfig.ip);
  DPRINT(F(" - Static Mask : ")); DPRINTLN(NetConfig.netmask);
  DPRINT(F(" - Static GW   : ")); DPRINTLN(NetConfig.gw);
}

bool loadSystemConfig() {
  DPRINTLN(F("- LOADING CONFIG"));
  Preferences configPreferences;
  configPreferences.begin("config", false);
  String hostname = configPreferences.getString("hostname", "");
  if (hostname == "") {
    DPRINTLN(F("Hostname is empty. Maybe a fresh ESP32 installation? Start config mode..."));
    configPreferences.end();
    return false;
  }

  hostname.toCharArray(NetConfig.hostname, VARIABLESIZE, 0);
  configPreferences.getString("ntp", DEFAULT_NTP_SERVER).toCharArray(NetConfig.ntp, VARIABLESIZE, 0);
  configPreferences.getString("ccuip", "").toCharArray(HomeMaticConfig.ccuIP, IPSIZE, 0);

  configPreferences.getString("ip", "0.0.0.0").toCharArray(NetConfig.ip, IPSIZE, 0);
  configPreferences.getString("netmask", "0.0.0.0").toCharArray(NetConfig.netmask, IPSIZE, 0);
  configPreferences.getString("gw", "0.0.0.0").toCharArray(NetConfig.gw, IPSIZE, 0);

  configPreferences.end();
  dumpSystemConfig();
  return true;
}

bool saveSystemConfig() {
  DPRINTLN(F(" - saveSystemConfig(): saving config"));
  Preferences configPreferences;
  configPreferences.begin("config", false);
  configPreferences.putString("ip", NetConfig.ip);
  configPreferences.putString("netmask", NetConfig.netmask);
  configPreferences.putString("gw", NetConfig.gw);
  configPreferences.putString("ntp", NetConfig.ntp);
  configPreferences.putString("hostname", NetConfig.hostname);
  configPreferences.putString("ccuip", HomeMaticConfig.ccuIP);
  delay(300);
  configPreferences.end();
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
