//- -----------------------------------------------------------------------------------------------------------------------
// AskSinAnalyzer
// 2019-06-01 jp112sdl Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
// 2019-06-01 psi-4ward Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------

#ifndef __WEB__H_
#define __WEB__H_

#include "Web_HTML.h"

AsyncWebServer webServer(80);

void setConfig(AsyncWebServerRequest *request) {
  DPRINTLN(F("- setConfig"));
  if (request->hasParam("ccuip")) {
    AsyncWebParameter* p = request->getParam("ccuip");
    p->value().toCharArray(HomeMaticConfig.ccuIP, IPSIZE, 0);
    DPRINT(F("  - ccuip: ")); DPRINTLN(HomeMaticConfig.ccuIP);
  }

  if (request->hasParam("ntp")) {
    AsyncWebParameter* p = request->getParam("ntp");
    p->value().toCharArray(NetConfig.ntp, VARIABLESIZE, 0);
    DPRINT(F("  - ntp: ")); DPRINTLN(NetConfig.ntp);
  }

  if (request->hasParam("hostname")) {
    AsyncWebParameter* p = request->getParam("hostname");
    p->value().toCharArray(NetConfig.hostname, VARIABLESIZE, 0);
    DPRINT(F("  - hostname: ")); DPRINTLN(NetConfig.hostname);
  }

  if (request->hasParam("ip")) {
    AsyncWebParameter* p = request->getParam("ip");
    p->value().toCharArray(NetConfig.ip, IPSIZE, 0);
    DPRINT(F("  - ip: ")); DPRINTLN(NetConfig.ip);
  }

  if (request->hasParam("netmask")) {
    AsyncWebParameter* p = request->getParam("netmask");
    p->value().toCharArray(NetConfig.netmask, IPSIZE, 0);
    DPRINT(F("  - netmask: ")); DPRINTLN(NetConfig.netmask);
  }

  if (request->hasParam("gw")) {
    AsyncWebParameter* p = request->getParam("gw");
    p->value().toCharArray(NetConfig.gw, IPSIZE, 0);
    DPRINT(F("  - gw: ")); DPRINTLN(NetConfig.gw);
  }

  DPRINTLN(F("- setConfig END"));

  bool ok = saveSystemConfig();

  String page = ok ? F("saveSystemConfig OK\n") : F("saveSystemConfig NOT OK\n");

  AsyncWebServerResponse *response = request->beginResponse(200);
  response->addHeader("Content-Length", String(page.length()));
  request->send(200, "text/plain", page);
}

void getConfig (AsyncWebServerRequest *request) {
  DPRINTLN(F("::: Web.h /getConfig"));
  bool staticipconfig = String(NetConfig.ip) != "0.0.0.0";
  String json = "{";
  json += "\"staticipconfig\":" + String(staticipconfig);
  json += ",";
  json += "\"ip\":\"" +  String(WiFi.localIP().toString()) + "\"";
  json += ",";
  json += "\"ntp\":\"" +  String(NetConfig.ntp) + "\"";
  json += ",";
  json += "\"hostname\":\"" +  String(NetConfig.hostname) + "\"";
  json += ",";
  json += "\"netmask\":\"" + String(WiFi.subnetMask().toString()) + "\"";
  json += ",";
  json += "\"gateway\":\"" + String(WiFi.gatewayIP().toString()) + "\"";
  json += ",";
  json += "\"macaddress\":\"" + String(WiFi.macAddress()) + "\"";
  json += ",";
  json += "\"ccuip\":\"" + String(HomeMaticConfig.ccuIP) + "\"";
  json += ",";
  json += "\"resolve\":" + String(RESOLVE_ADDRESS);
  json += ",";
  json += "\"sdcardavailable\":" + String(sdAvailable);
  json += ",";
  json += "\"sdcardsizemb\":" + String(getSDCardSizeMB());
  json += ",";
  json += "\"sdcardtotalspacemb\":\"" + String(getSDCardTotalSpaceMB()) + "\"";
  json += ",";
  json += "\"sdcardusedspacemb\":\"" + String(getSDCardUsedSpaceMB()) + "\"";
  json += ",";
  json += "\"spiffssizekb\":" + String(getSPIFFSSizeKB());
  json += ",";
  json += "\"spiffsusedkb\":" + String(getSPIFFSUsedKB());
  json += ",";
  json += "\"boottime\":" + String(bootTime);
  json += ",";
  json += "\"display\":" + String(HAS_DISPLAY);
  json += ",";
  json += "\"version_upper\":" + String(VERSION_UPPER);
  json += ",";
  json += "\"version_lower\":" + String(VERSION_LOWER);
  json += "}";
  DPRINT("/getConfig JSON: "); DPRINTLN(json);
  AsyncWebServerResponse *response = request->beginResponse(200);
  response->addHeader("Content-Length", String(json.length()));
  request->send(200, "application/json", json);
}

void getAskSinAnalyzerDevList (AsyncWebServerRequest *request) {
  DPRINTLN(F("::: Web.h /getAskSinAnalyzerDevList"));
  AsyncResponseStream *response = request->beginResponseStream("application/xml;charset=iso-8859-1");
  HTTPClient http;
  WiFiClient client;
  http.begin(client, "http://" + String(HomeMaticConfig.ccuIP) + ":8181/ret.exe?ret=dom.GetObject(\""+CCU_SV+"\").Value()");
  int httpCode = http.GET();
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      int len = http.getSize();
      uint8_t buff[128] = { 0 };
      WiFiClient * stream = &client;
      while (http.connected() && (len > 0 || len == -1)) {
        int c = stream->readBytes(buff, std::min((size_t)len, sizeof(buff)));
        if (!c) DPRINTLN(F("getAskSinAnalyzerDevList read timeout"));

        for (uint8_t a = 0; a < c; a++)
          response->print((char)buff[a]);
        if (len > 0)  len -= c;
      }
    } else {
      DPRINT(F("::: getAskSinAnalyzerDevList HTTP GET ERROR ")); DDECLN(httpCode);
    }
  } else {
    DPRINT(F(":::getAskSinAnalyzerDevList HTTP-Client failed with ")); DDECLN(httpCode);
  }
  http.end();
  request->send(response);
}

void getAskSinAnalyzerDevListJSON (AsyncWebServerRequest *request) {
  DPRINTLN(F("::: Web.h /getAskSinAnalyzerDevListJSON"));
  AsyncResponseStream *response = request->beginResponseStream("application/json;charset=iso-8859-1");
  String js = loadAskSinAnalyzerDevListFromCCU();
  createJSONDevList(js);  //refresh local DevList
  response->print(js);    //send DevList to Web
  request->send(response);
}

void getLogByLogNumber (AsyncWebServerRequest * request) {
  int32_t lognum = 0;
  if (request->hasParam("lognum")) {
    AsyncWebParameter* p = request->getParam("lognum");
    lognum = p->value().toInt();
  }

  AsyncResponseStream *response = request->beginResponseStream("application/json");
  response->print("[");
  for (uint16_t l = 0; l < logLength; l++) {
    if ((int32_t)LogTable[l].lognumber > lognum && l < MAX_LOG_ENTRIES) {
      String json = "";
      if (l > 0) json += ",";
      json += "{";
      json += "\"lognumber\": " + String(LogTable[l].lognumber) + ", ";
      json += "\"tstamp\": " + String(LogTable[l].time) + ", ";
      json += "\"rssi\": " + String(LogTable[l].rssi) + ", ";
      String from = String(LogTable[l].fromAddress);
      from.trim();
      json += "\"from\": \"" + from + "\", ";
      String to = String(LogTable[l].toAddress);
      to.trim();
      json += "\"to\": \"" + to + "\", ";
      json += "\"len\": " + String(LogTable[l].len) + ", ";
      json += "\"cnt\": " + String(LogTable[l].cnt) + ", ";
      String t = String(LogTable[l].typ);
      t.trim();
      json += "\"typ\": \"" + t + "\", ";
      String fl = String(LogTable[l].flags);
      fl.trim();
      json += "\"flags\": \"" + fl + "\"";
      json += "}";
      response->print(json);
    }
    if (l == MAX_LOG_ENTRIES) break;
  }

  response->print("]");
  request->send(response);
}

void indexHtml(AsyncWebServerRequest * request) {
  String page = FPSTR(HTTP_INDEX);

  page.replace("{branch}", WEB_BRANCH);

  AsyncWebServerResponse *response = request->beginResponse(200);
  response->addHeader("Content-Length", String(page.length()));
  request->send(200, "text/html", page);
}

void setBootConfigMode(AsyncWebServerRequest * request) {
  if (SPIFFS.begin()) {
    DPRINTLN(F("setBootConfigMode mounted file system"));
    if (!SPIFFS.exists(BOOTCONFIGMODE_FILENAME)) {
      File bootConfigModeFile = SPIFFS.open(BOOTCONFIGMODE_FILENAME, "w");
      bootConfigModeFile.print("0");
      bootConfigModeFile.close();
      SPIFFS.end();
      DPRINTLN(F("Boot to ConfigMode requested. Restarting..."));
      request->send(200, "text/plain", F("<state>enableBootConfigMode - Rebooting</state>"));
      delay(500);
      ESP.restart();
    } else {
      request->send(200, "text/plain", F("<state>enableBootConfigMode - FAILED!</state>"));
      SPIFFS.end();
    }
  }
}

void checkUpdate(String url) {
  if (updating == true) {
    updating = false;
    DPRINTLN(F("Check for Updates..."));

    digitalWrite(AP_MODE_LED_PIN, HIGH);
    ESPhttpUpdate.rebootOnUpdate(false);
    t_httpUpdate_return ret = ESPhttpUpdate.update(url);

    switch (ret) {
      case HTTP_UPDATE_FAILED:
        DPRINT(F("HTTP_UPDATE_FAILED Error")); DDEC(ESPhttpUpdate.getLastError()); DPRINT(F(":")); DPRINTLN(ESPhttpUpdate.getLastErrorString());
        DPRINTLN(F(""));
        break;

      case HTTP_UPDATE_NO_UPDATES:
        DPRINTLN(F("HTTP_UPDATE_NO_UPDATES"));
        break;

      case HTTP_UPDATE_OK:
        DPRINTLN(F("HTTP_UPDATE_OK. Rebooting..."));
        delay(200);
        ESP.restart();
        break;
    }

    digitalWrite(AP_MODE_LED_PIN, LOW);

  }
}

void httpUpdate(AsyncWebServerRequest * request) {
  String url = "";
  if (request->hasParam("url")) {
    AsyncWebParameter* p = request->getParam("url");
    url = p->value();
  }
  String page = "Processing update from " + url + "\nPlease be patient - ESP32 will reboot automatically";
  AsyncWebServerResponse *response = request->beginResponse(200);
  response->addHeader("Content-Length", String(page.length()));
  request->send(200, "text/plain", page);



  if (url.length() > 10) {
    updateUrl = url;
    updating = true;
  }
}

void initWebServer() {
  webServer.on("/reboot", HTTP_POST, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", "rebooting");
    delay(100);
    ESP.restart();
  });

  webServer.on("/getConfig", HTTP_GET, [](AsyncWebServerRequest * request) {
    getConfig(request);
  });

  webServer.on("/setConfig", HTTP_GET, [](AsyncWebServerRequest * request) {
    setConfig(request);
  });

  webServer.on("/rebootInConfigMode", HTTP_POST, [](AsyncWebServerRequest * request) {
    setBootConfigMode(request);
  });

  webServer.on("/getAskSinAnalyzerDevList", HTTP_GET, [](AsyncWebServerRequest * request) {
    getAskSinAnalyzerDevList(request);
  });

  webServer.on("/getAskSinAnalyzerDevListJSON", HTTP_GET, [](AsyncWebServerRequest * request) {
    getAskSinAnalyzerDevListJSON(request);
  });

  webServer.on("/getLogByLogNumber", HTTP_GET, [](AsyncWebServerRequest * request) {
    getLogByLogNumber(request);
  });

  webServer.on("/httpupdate", HTTP_GET, [](AsyncWebServerRequest * request) {
    httpUpdate(request);
  });

  webServer.on("/deletecsv", HTTP_POST, [](AsyncWebServerRequest * request) {
    bool backup = false;
    if (request->hasParam("backup")) {
      AsyncWebParameter* p = request->getParam("backup");
      backup = (p->value() == "1");
    }
    deleteCSV(CSV_FILENAME, backup);
    request->send(200, "text/plain", "csv deleted, " + (String)((backup == true) ? "with" : "without") + " backup");
  });

  webServer.on("/downloadcsv", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response;
    if (sdAvailable) {
      DPRINTLN(F("Downloading CSV from SD Card"));
      response = request->beginResponse(SD, CSV_FILENAME, String());
    }
    else {
      DPRINTLN(F("Downloading CSV from SPIFFS"));
      response = request->beginResponse(SPIFFS, CSV_FILENAME, String());
    }
    response->addHeader("Server", "AskSinAnalyzer");
    request->send(response);
  });

  webServer.on("/dl", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (request->hasArg("filename")) {
      String fileName =  request->arg("filename");
      if (!fileName.startsWith("/"))
        fileName = "/" + fileName;
      if (SPIFFS.exists(fileName)) {
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, fileName, String());
        response->addHeader("Server", "AskSinAnalyzer");
        request->send(response);
      } else {
        request->send(404, "text/plain", "file " + fileName + " not found");
      }
    }
  });

  webServer.onNotFound([](AsyncWebServerRequest * request) {
    indexHtml(request);
  });

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  webServer.begin();
  MDNS.addService("http", "tcp", 80);
}

bool initmDNS() {
  return MDNS.begin(NetConfig.hostname);
}
#endif
