#ifndef WIFIFUNCTIONS_H_
#define WIFIFUNCTIONS_H_

void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress IP = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(IP);
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

bool wifiConnect() {
  drawStatusCircle(ILI9341_YELLOW);
  WiFi.begin(WIFI_SSID, WIFI_PSK);
  uint8_t connect_count = 0;
  Serial.println("Connecting to WiFi.");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    connect_count++;
    if (connect_count > 60) return false;
  }
  connect_count = 0;
  Serial.println("\nConnected to the WiFi network");
  printWifiStatus();
  return true;
}

#endif
