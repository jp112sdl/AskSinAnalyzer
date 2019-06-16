//- -----------------------------------------------------------------------------------------------------------------------
// AskSinAnalyzer
// 2019-06-01 jp112sdl Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------

#ifndef NTP_H_
#define NTP_H_

const int NTP_PACKET_SIZE = 48;
byte packetBuffer[ NTP_PACKET_SIZE];
unsigned int localPort = 2390;
WiFiUDP udp;

unsigned long sendNTPpacket(IPAddress& address) {
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;
  packetBuffer[1] = 0;
  packetBuffer[2] = 6;
  packetBuffer[3] = 0xEC;
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  udp.beginPacket(address, 123);
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

time_t getNtpTime() {
  IPAddress ntpServerIP;
  while (udp.parsePacket() > 0) ;
  WiFi.hostByName(NetConfig.ntp, ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1600) {
    int size = udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      udp.read(packetBuffer, NTP_PACKET_SIZE);
      unsigned long secsSince1900;
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      DPRINTLN(" - NTP time was set from " + String(NetConfig.ntp));
      return secsSince1900 - 2208988800UL;
    }
  }
  return 0;
}

bool doNTPinit() {
  if (isOnline) {
    udp.begin(localPort);
    setSyncProvider(getNtpTime);
    setSyncInterval(3600);
    byte timeSetRetries = 0;
    while (timeStatus() == timeNotSet) {
      DPRINTLN(" - Waiting for Time from " + String(NetConfig.ntp));
      delay(2000);
      if (timeSetRetries > 5) {
        DPRINTLN(" - Timeout!");
        //DPRINTLN("Rebooting");
        //delay(2000);
        //ESP.restart();
        return false;
      }
      timeSetRetries++;
    }
    return true;
  } else return false;
}

boolean summertime(time_t t, byte tzHours) {
  if (month(t) < 3 || month(t) > 10) return false; // keine Sommerzeit in Jan, Feb, Nov, Dez
  if (month(t) > 3 && month(t) < 10) return true; // Sommerzeit in Apr, Mai, Jun, Jul, Aug, Sep
  if (month(t) == 3 && (hour(t) + 24 * day(t)) >= (1 + tzHours + 24 * (31 - (5 * year(t) / 4 + 4) % 7)) || month(t) == 10 && (hour(t) + 24 * day(t)) < (1 + tzHours + 24 * (31 - (5 * year(t) / 4 + 1) % 7)))
    return true;
  else
    return false;
}

String getUhrzeit(time_t t) {
  byte stunde = hour(t) + 1;
  stunde = (summertime(t, 0)) ? stunde + 1 : stunde;
  String Stunde = (stunde < 10) ? "0" + String(stunde) : String(stunde);
  String Minute = (minute(t) < 10) ? "0" + String(minute(t)) : String(minute(t));
  String Sekunde = (second(t) < 10) ? "0" + String(second(t)) : String(second(t));
  return Stunde + ":" + Minute + ":" + Sekunde;
}

String getDatum(time_t t) {
  String Tag = (day(t) < 10) ? "0" + String(day(t)) : String(day(t));
  String Monat = (month(t) < 10) ? "0" + String(month(t)) : String(month(t));
  String Jahr = String(year(t));
  return Tag + "." + Monat + "." + Jahr;
}

#endif
