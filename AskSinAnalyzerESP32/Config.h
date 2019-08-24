//- -----------------------------------------------------------------------------------------------------------------------
// AskSinAnalyzer
// 2019-06-01 jp112sdl Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------
#ifndef CONFIG_H_
#define CONFIG_H_

#define CONFIG                        "config"
#define CONFIG_IP                     "ip"
#define CONFIG_NETMASK                "netmask"
#define CONFIG_GW                     "gw"
#define CONFIG_HOSTNAME               "hostname"
#define CONFIG_NTP                    "ntp"
#define CONFIG_CCUIP                  "ccuip"
#define CONFIG_BOOTCONFIGMODE         "bootConfigMode"

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
  configPreferences.begin(CONFIG, false);
  String hostname = configPreferences.getString(CONFIG_HOSTNAME, "");
  if (hostname == "") {
    DPRINTLN(F("Hostname is empty. Maybe a fresh ESP32 installation? Start config mode..."));
    configPreferences.end();
    return false;
  }

  hostname.toCharArray(NetConfig.hostname, VARIABLESIZE, 0);
  configPreferences.getString(CONFIG_NTP, DEFAULT_NTP_SERVER).toCharArray(NetConfig.ntp, VARIABLESIZE, 0);
  configPreferences.getString(CONFIG_CCUIP, "").toCharArray(HomeMaticConfig.ccuIP, IPSIZE, 0);

  configPreferences.getString(CONFIG_IP, "0.0.0.0").toCharArray(NetConfig.ip, IPSIZE, 0);
  configPreferences.getString(CONFIG_NETMASK, "0.0.0.0").toCharArray(NetConfig.netmask, IPSIZE, 0);
  configPreferences.getString(CONFIG_GW, "0.0.0.0").toCharArray(NetConfig.gw, IPSIZE, 0);

  configPreferences.end();
  dumpSystemConfig();
  return true;
}

bool saveSystemConfig() {
  DPRINTLN(F(" - saveSystemConfig(): saving config"));
  Preferences configPreferences;
  configPreferences.begin(CONFIG, false);
  configPreferences.putString(CONFIG_IP, NetConfig.ip);
  configPreferences.putString(CONFIG_NETMASK, NetConfig.netmask);
  configPreferences.putString(CONFIG_GW, NetConfig.gw);
  configPreferences.putString(CONFIG_NTP, NetConfig.ntp);
  configPreferences.putString(CONFIG_HOSTNAME, NetConfig.hostname);
  configPreferences.putString(CONFIG_CCUIP, HomeMaticConfig.ccuIP);
  delay(300);
  configPreferences.end();
  return true;
}

bool bootConfigMode() {
  Preferences configPreferences;
  configPreferences.begin(CONFIG, false);
  bool b = configPreferences.getBool(CONFIG_BOOTCONFIGMODE, false);
  configPreferences.end();
  return b;
}

void bootConfigMode(bool b) {
  Preferences configPreferences;
  configPreferences.begin(CONFIG, false);
  configPreferences.putBool(CONFIG_BOOTCONFIGMODE, b);
  delay(300);
  configPreferences.end();
}

#endif
