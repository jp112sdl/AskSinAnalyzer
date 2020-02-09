//- -----------------------------------------------------------------------------------------------------------------------
// AskSinAnalyzer
// 2019-06-01 jp112sdl Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------

#ifndef WIFIFUNCTIONS_H_
#define WIFIFUNCTIONS_H_

unsigned long lastReconnectMillis = 0;

Preferences wifiPreferences;

//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  DPRINTLN("Should save config");
  shouldSaveConfig = true;
}
void printWifiStatus() {
  DPRINT(" - SSID: ");
  DPRINTLN(WiFi.SSID());
  IPAddress IP = WiFi.localIP();
  DPRINT(" - IP Address: ");
  DPRINTLN(IP);
  long rssi = WiFi.RSSI();
  DPRINT(" - RSSI: ");
  DPRINT(rssi);
  DPRINTLN(" dBm");
  DPRINT(" - Hostname: ");
  DPRINTLN(WiFi.getHostname());
}

void checkWifi() {
  if (isOnline && ( WiFi.status() != WL_CONNECTED || WiFi.SSID().length() < 2)) {
    if (millis() - lastReconnectMillis > 10000) {
      DPRINTLN("Wifi disconnected. Reconnect initiated.");
      WiFi.disconnect();
      WiFi.reconnect();
      lastReconnectMillis = millis();
    }
  }
}

bool doWifiConnect() {
  DPRINTLN(F("- INIT WIFI CONNECT."));
  wifiPreferences.begin("wifi", false);
  String _ssid = wifiPreferences.getString("ssid", "none");      //NVS key ssid
  String _psk = wifiPreferences.getString("password", "none");   //NVS key password
  wifiPreferences.end();


  //DPRINTLN("ssid; " + _ssid);
  //DPRINTLN(" psk; " + _psk);


  String _pskMask = "";
  for (uint8_t i = 0; i < _psk.length(); i++) {
    _pskMask += "*";
  }
  const char* ipStr = NetConfig.ip; byte ipBytes[4]; parseBytes(ipStr, '.', ipBytes, 4, 10);
  const char* netmaskStr = NetConfig.netmask; byte netmaskBytes[4]; parseBytes(netmaskStr, '.', netmaskBytes, 4, 10);
  const char* gwStr = NetConfig.gw; byte gwBytes[4]; parseBytes(gwStr, '.', gwBytes, 4, 10);

  if (!startWifiManager && _ssid != "" && _psk != "" ) {
    digitalWrite(AP_MODE_LED_PIN, LOW);
    if (String(NetConfig.ip) != "0.0.0.0") {
      WiFi.config(IPAddress(ipBytes[0], ipBytes[1], ipBytes[2], ipBytes[3]), IPAddress(gwBytes[0], gwBytes[1], gwBytes[2], gwBytes[3]), IPAddress(netmaskBytes[0], netmaskBytes[1], netmaskBytes[2], netmaskBytes[3]), IPAddress(gwBytes[0], gwBytes[1], gwBytes[2], gwBytes[3]));
    }
#ifdef USE_DISPLAY
    drawStatusCircle(ILI9341_YELLOW);
#endif
    WiFi.setHostname(NetConfig.hostname);     // <- better here?
    WiFi.begin(_ssid.c_str(), _psk.c_str());
    WiFi.setHostname(NetConfig.hostname);     // <- or here ?
    uint8_t connect_count = 0;
    DPRINT(F(" - Connecting to WiFi"));
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      DPRINT(".");
      connect_count++;
      if (connect_count > 60) {
        DPRINTLN("");
        return false;
      }
    }
    connect_count = 0;
    DPRINTLN(F("\n - Connected to the WiFi network"));
    printWifiStatus();
    return true;
  } else {
    digitalWrite(AP_MODE_LED_PIN, HIGH);
#ifdef USE_DISPLAY
    showAPModeDisplay();
#endif
    WiFiManager wifiManager;
    WiFiManagerParameter custom_ip("custom_ip", "IP-Adresse", (String(NetConfig.ip) != "0.0.0.0") ? NetConfig.ip : "", IPSIZE, 0, "pattern='((^|\\.)((25[0-5])|(2[0-4]\\d)|(1\\d\\d)|([1-9]?\\d))){4}$'");
    WiFiManagerParameter custom_netmask("custom_netmask", "Netzmaske", (String(NetConfig.netmask) != "0.0.0.0") ? NetConfig.netmask : "", IPSIZE, 0, "pattern='((^|\\.)((25[0-5])|(2[0-4]\\d)|(1\\d\\d)|([1-9]?\\d))){4}$'");
    WiFiManagerParameter custom_gw("custom_gw", "Gateway",  (String(NetConfig.gw) != "0.0.0.0") ? NetConfig.gw : "", IPSIZE, 0, "pattern='((^|\\.)((25[0-5])|(2[0-4]\\d)|(1\\d\\d)|([1-9]?\\d))){4}$'");
    WiFiManagerParameter custom_hostname("custom_hostname", "Hostname (leave blank to reset to default)", NetConfig.hostname, VARIABLESIZE, 0, "pattern='[A-Za-z0-9_ -.]+'");
    WiFiManagerParameter custom_ntp("custom_ntp", "NTP-Server (leave blank to reset to default)", NetConfig.ntp, VARIABLESIZE, 0, "pattern='[A-Za-z0-9_ -.]+'");
    WiFiManagerParameter custom_ccuip("ccu", "CCU IP", HomeMaticConfig.ccuIP, IPSIZE, 0, "pattern='((^|\\.)((25[0-5])|(2[0-4]\\d)|(1\\d\\d)|([1-9]?\\d))){4}$'");
    WiFiManagerParameter custom_backendurl("backendurl", "Backend URL", HomeMaticConfig.backendUrl, VARIABLESIZE, 0, "");

    String backend = "";
    switch (HomeMaticConfig.backendType) {
      case BT_CCU:
        backend = F("<option selected value='0'>CCU</option><option value='1'>Other</option>");
        break;
      case BT_OTHER:
        backend = F("<option value='0'>CCU</option><option selected value='1'>Other</option>");
        break;
      default:
        backend = F("<option value='0'>CCU</option><option value='1'>Other</option>");
        break;
    }
    WiFiManagerParameter custom_backendtype("backendtype", "Backend", "", 8, 2, backend.c_str());

    wifiManager.setSaveConfigCallback(saveConfigCallback);

    //set static ip
    //wifiManager.setSTAStaticIPConfig(IPAddress(10, 0, 1, 99), IPAddress(10, 0, 1, 1), IPAddress(255, 255, 255, 0));
    WiFi.mode(WIFI_STA);

    wifiManager.addParameter(&custom_backendtype);
    wifiManager.addParameter(&custom_ccuip);
    wifiManager.addParameter(&custom_backendurl);
    wifiManager.addParameter(&custom_ip);
    wifiManager.addParameter(&custom_netmask);
    wifiManager.addParameter(&custom_gw);
    wifiManager.addParameter(&custom_hostname);
    wifiManager.addParameter(&custom_ntp);

    if (!wifiManager.startConfigPortal("AskSinAnalyzer-AP")) {
      DPRINTLN("failed to connect and hit timeout");
      delay(3000);
      ESP.restart();
      delay(5000);
    }

    DPRINTLN("Wifi Connected");

    if (shouldSaveConfig) {
      wifiPreferences.begin("wifi", false); // Note: Namespace name is limited to 15 chars
      DPRINTLN("Writing new ssid");
      wifiPreferences.putString("ssid", WiFi.SSID());

      DPRINTLN("Writing new pass");
      wifiPreferences.putString("password", WiFi.psk());
      delay(300);
      wifiPreferences.end();

      if (String(custom_ip.getValue()).length() > 5) {
        DPRINTLN("Custom static IP Address is set!");
        strcpy(NetConfig.ip, custom_ip.getValue());
        strcpy(NetConfig.netmask, custom_netmask.getValue());
        strcpy(NetConfig.gw, custom_gw.getValue());

      } else {
        strcpy(NetConfig.ip,      "0.0.0.0");
        strcpy(NetConfig.netmask, "0.0.0.0");
        strcpy(NetConfig.gw,      "0.0.0.0");
      }

      if (String(custom_ntp.getValue()).length() > 1 && strcmp(custom_ntp.getValue(), "null") != 0) {
        strcpy(NetConfig.ntp, custom_ntp.getValue());
      } else {
        strcpy(NetConfig.ntp, DEFAULT_NTP_SERVER);
      }

      if (String(custom_hostname.getValue()).length() > 1 && strcmp(custom_hostname.getValue(), "null") != 0) {
        strcpy(NetConfig.hostname, custom_hostname.getValue());
      } else {
        strcpy(NetConfig.hostname, DEFAULT_HOSTNAME);
      }

      strcpy(HomeMaticConfig.ccuIP, custom_ccuip.getValue());
      strcpy(HomeMaticConfig.backendUrl, custom_backendurl.getValue());
      HomeMaticConfig.backendType = (atoi(custom_backendtype.getValue()));

      saveSystemConfig();
      digitalWrite(AP_MODE_LED_PIN, LOW);
    }
  }
  return true;
}

#endif
