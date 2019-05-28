#ifndef WIFIFUNCTIONS_H_
#define WIFIFUNCTIONS_H_

bool wifiConnect() {
  drawStatusCircle(ILI9341_YELLOW);
  WiFi.begin(WIFI_SSID, WIFI_PSK);
  uint8_t connect_count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
    connect_count++;
    if (connect_count > 60) return false;
  }
  connect_count = 0;
  Serial.println("Connected to the WiFi network");
  return true;
}

#endif
