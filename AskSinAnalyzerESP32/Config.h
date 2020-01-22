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
#define CONFIG_BACKEND                "backend"
#define CONFIG_BACKEND_URL            "backendurl"
#define CONFIG_BOOTCONFIGMODE         "bootConfigMode"
#define CONFIG_RSSI_HISTOGRAMBARWIDTH "rssi_hbw"

void dumpSystemConfig() {
  DPRINTLN(F("- dump config:"));
  DPRINT(F(" - Hostname    : ")); DPRINTLN(NetConfig.hostname);
  DPRINT(F(" - BACKEND     : ")); DPRINTLN(HomeMaticConfig.backendType);
  DPRINT(F(" - BACKEND URL : ")); DPRINTLN(HomeMaticConfig.backendUrl);
  DPRINT(F(" - CCU IP      : ")); DPRINTLN(HomeMaticConfig.ccuIP);
  DPRINT(F(" - NTP         : ")); DPRINTLN(NetConfig.ntp);
  DPRINT(F(" - Static IP   : ")); DPRINTLN(NetConfig.ip);
  DPRINT(F(" - Static Mask : ")); DPRINTLN(NetConfig.netmask);
  DPRINT(F(" - Static GW   : ")); DPRINTLN(NetConfig.gw);
  DPRINT(F(" - RSSI Hist.BW: ")); DPRINTLN(RSSIConfig.histogramBarWidth);
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
  HomeMaticConfig.backendType = configPreferences.getUChar(CONFIG_BACKEND, BT_CCU);
  configPreferences.getString(CONFIG_BACKEND_URL, "").toCharArray(HomeMaticConfig.backendUrl, VARIABLESIZE, 0);

  configPreferences.getString(CONFIG_IP, "0.0.0.0").toCharArray(NetConfig.ip, IPSIZE, 0);
  configPreferences.getString(CONFIG_NETMASK, "0.0.0.0").toCharArray(NetConfig.netmask, IPSIZE, 0);
  configPreferences.getString(CONFIG_GW, "0.0.0.0").toCharArray(NetConfig.gw, IPSIZE, 0);

  RSSIConfig.histogramBarWidth = configPreferences.getUChar(CONFIG_RSSI_HISTOGRAMBARWIDTH, 5);

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
  configPreferences.putString(CONFIG_BACKEND_URL, HomeMaticConfig.backendUrl);
  configPreferences.putUChar(CONFIG_BACKEND, HomeMaticConfig.backendType);
  configPreferences.putUChar(CONFIG_RSSI_HISTOGRAMBARWIDTH, RSSIConfig.histogramBarWidth);
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
