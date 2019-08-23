//- -----------------------------------------------------------------------------------------------------------------------
// AskSinAnalyzer
// 2019-06-01 jp112sdl Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
// 2019-06-01 psi-4ward Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------
//AsyncTCP Commit [ff5c8b2]01db9cf9cea0d62e42d6c1a62dbd4b53d 22.06.2019

#ifndef __WEB__H_
#define __WEB__H_

#include "Web_HTML.h"

AsyncWebServer webServer(80);
AsyncWebSocket ws("/ws");
#define MAX_WSCLIENTS 3
AsyncWebSocketClient * wsClients[MAX_WSCLIENTS] = {NULL};

void writeLogEntryToWebSocket(const _LogTable &lt) {
  for (AsyncWebSocketClient * wsClient : wsClients)  {
    if (wsClient != NULL) {
      String json = "{";
      json += "\"lognumber\": " + String(lt.lognumber) + ", ";
      json += "\"tstamp\": " + String(lt.time) + ", ";
      json += "\"rssi\": " + String(lt.rssi) + ", ";
      String from = String(lt.fromAddress);
      from.trim();
      json += "\"from\": \"" + from + "\", ";
      String to = String(lt.toAddress);
      to.trim();
      json += "\"to\": \"" + to + "\", ";
      json += "\"len\": " + String(lt.len) + ", ";
      json += "\"cnt\": " + String(lt.cnt) + ", ";
      String t = String(lt.typ);
      t.trim();
      json += "\"typ\": \"" + t + "\", ";
      String fl = String(lt.flags);
      fl.trim();
      json += "\"flags\": \"" + fl + "\"";
      json += "}";
      wsClient->text(json);
    }
  }
}

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    bool clientAdded = false;
    for (uint8_t i = 0; i < MAX_WSCLIENTS; i++) {
      if (wsClients[i] == NULL) {
        wsClients[i] = client;

        /*for (uint16_t l = 0; l < logLength; l++) {
          if ((int32_t)LogTable[l].lognumber > lognum && l < MAX_LOG_ENTRIES) {
            writeLogEntryToWebSocket(LogTable[logLength - l - 1]);
          }
          if (l == MAX_LOG_ENTRIES) break;
          }*/

        clientAdded = true;
        DPRINT(F("- wsClient Connect: ID ")); DDEC(client->id()); DPRINT(F(" from ")); DPRINTLN(client->remoteIP());
        break;
      }
    }
    if (!clientAdded) {
      client->close();
      DPRINTLN(F("- wsClient Connect: NO FREE SLOTS"));
    }
  } else if (type == WS_EVT_DISCONNECT) {
    for (uint8_t i = 0; i < MAX_WSCLIENTS; i++) {
      if (wsClients[i] != NULL && wsClients[i]->id() == client->id()) {
        wsClients[i] = NULL;
        DPRINT("- wsClient Disconnect ID "); DDECLN(client->id());
        break;
      }
    }
  } else if (type == WS_EVT_ERROR) {
    DPRINT(F("-wsClient Error ")); DPRINTLN((char*)data);
  } else if (type == WS_EVT_DATA) {
    AwsFrameInfo * info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len) {
      DPRINT(F("WS_EVT_DATA: "));
      DPRINTLN((char*)data);
    }
  }
}

void setConfig(AsyncWebServerRequest *request) {
  DPRINTLN(F("- setConfig"));
  if (request->hasParam("ccuip", true)) {
    AsyncWebParameter* p = request->getParam("ccuip", true);
    p->value().toCharArray(HomeMaticConfig.ccuIP, IPSIZE, 0);
    DPRINT(F("  - ccuip: ")); DPRINTLN(HomeMaticConfig.ccuIP);
  }

  if (request->hasParam("ntp", true)) {
    AsyncWebParameter* p = request->getParam("ntp", true);
    p->value().toCharArray(NetConfig.ntp, VARIABLESIZE, 0);
    DPRINT(F("  - ntp: ")); DPRINTLN(NetConfig.ntp);
  }

  if (request->hasParam("hostname", true)) {
    AsyncWebParameter* p = request->getParam("hostname", true);
    p->value().toCharArray(NetConfig.hostname, VARIABLESIZE, 0);
    DPRINT(F("  - hostname: ")); DPRINTLN(NetConfig.hostname);
  }

  if (request->hasParam("ip", true)) {
    AsyncWebParameter* p = request->getParam("ip", true);
    p->value().toCharArray(NetConfig.ip, IPSIZE, 0);
    DPRINT(F("  - ip: ")); DPRINTLN(NetConfig.ip);
  }

  if (request->hasParam("netmask", true)) {
    AsyncWebParameter* p = request->getParam("netmask", true);
    p->value().toCharArray(NetConfig.netmask, IPSIZE, 0);
    DPRINT(F("  - netmask: ")); DPRINTLN(NetConfig.netmask);
  }

  if (request->hasParam("gw", true)) {
    AsyncWebParameter* p = request->getParam("gw", true);
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
  json += "\"spiffssizekb\":" + String(getFFatSizeKB());                                       //spiffssizekb due to webui compatibility, has to be changed in future
  json += ",";
  json += "\"spiffsusedkb\":" + String(getFFatUsedKB());                                       //spiffsusedkb due to webui compatibility, has to be changed in future
  json += ",";
  json += "\"boottime\":" + String(bootTime);
  json += ",";
  json += "\"display\":" + String(HAS_DISPLAY);
  json += ",";
  json += "\"version_upper\":" + String(VERSION_UPPER);
  json += ",";
  json += "\"version_lower\":" + String(VERSION_LOWER);
  json += "}";
  DPRINT(F("::: /getConfig JSON: ")); DPRINTLN(json);
  AsyncWebServerResponse *response = request->beginResponse(200);
  response->addHeader("Content-Length", String(json.length()));
  request->send(200, "application/json", json);
}

void getAskSinAnalyzerDevListJSON (AsyncWebServerRequest *request) {
  DPRINTLN(F("::: Web.h /getAskSinAnalyzerDevListJSON"));
  String js = loadAskSinAnalyzerDevListFromCCU();
  if (js != "null") {
    AsyncResponseStream *response = request->beginResponseStream("application/json;charset=iso-8859-1");
    createJSONDevList(js);  //refresh local DevList
    response->print(js);    //send DevList to Web
    request->send(response);
  } else {
    DPRINTLN(F("-> E: js == null"));
    request->send(422, "text/plain", "Fehler beim Abruf der SV");
  }
}

void getLogByLogNumber (AsyncWebServerRequest * request) {
  bool formatIsCSV = false;
  if (request->hasParam("format")) formatIsCSV = (request->getParam("format")->value() == "csv");

  int32_t lognum = 0;
  if (request->hasParam("lognum")) lognum = request->getParam("lognum")->value().toInt();

  if (formatIsCSV) {
    if (lognum == -1) {
      AsyncWebServerResponse *response;
      if (maxSessionFiles > 1) {
        DPRINT("Merging all logfiles to tempfile ");
        unsigned long startMillis = millis();
        File temp = FFat.open("/temp.log", FILE_WRITE);
        const uint8_t len = 255;
        uint8_t buf[256] = {0};
        for (uint8_t i = 0; i < maxSessionFiles; i++) {
          DPRINT(".");
          if (FFat.exists(getSessionFileName(i))) {
            File file = FFat.open(getSessionFileName(i).c_str(), FILE_READ);
            while (file.available()) {
              size_t available = file.available();
              file.read(buf, (available > len) ? len : available);
              temp.write(buf, (available > len) ? len : available);
            }
            file.close();
          }
        }
        temp.close();
        DPRINT(" done. duration (ms): ");DDECLN(millis() - startMillis);
        response = request->beginResponse(FFat, "/temp.log", "text/comma-separated-values");
      } else {
        response = request->beginResponse(FFat, "/0.log", "text/comma-separated-values");
      }
      request->send(response);

    } else {
      AsyncResponseStream *response = request->beginResponseStream("text/comma-separated-values");
      for (uint16_t l = 0; l < logLength; l++) {
        if ((int32_t)LogTable[l].lognumber > lognum && l < MAX_LOG_ENTRIES) {
          response->println(createCSVFromLogTableEntry(LogTable[l], false));
        }
        if (l == MAX_LOG_ENTRIES) break;
      }
      request->send(response);
    }
  } else {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    response->print("[");
    for (uint16_t l = 0; l < logLength; l++) {
      if ((int32_t)LogTable[l].lognumber > lognum && l < MAX_LOG_ENTRIES) {
        String json = "";
        if (l > 0) json += ",";
        json += createJSONFromLogTableEntry(LogTable[l]);
        response->print(json);
      }
      if (l == MAX_LOG_ENTRIES) break;
    }
    response->print("]");
    request->send(response);
  }
}

void indexHtml(AsyncWebServerRequest * request) {
  String page = FPSTR(HTTP_INDEX);

  page.replace("{branch}", WEB_BRANCH);

  AsyncWebServerResponse *response = request->beginResponse(200);
  response->addHeader("Content-Length", String(page.length()));
  request->send(200, "text/html", page);
}

void setBootConfigMode(AsyncWebServerRequest * request) {
  bootConfigMode(true);
  DPRINTLN(F("Boot to ConfigMode requested. Restarting..."));
  request->send(200, "text/plain", F("enableBootConfigMode - Rebooting"));
  delay(500);
  ESP.restart();
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

void formatFFat(AsyncWebServerRequest * request) {
  String text = F("Formatted FFat done.");
  AsyncWebServerResponse *response = request->beginResponse(200);
  response->addHeader("Content-Length", String(text.length()));

  FFat.format();
  initSessionLogOnFFat();
  request->send(200, "text/plain", text);
}

void httpUpdate(AsyncWebServerRequest * request) {
  String url = "";
  if (request->hasParam("url")) url = request->getParam("url")->value();

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

  webServer.on("/setConfig", HTTP_POST, [](AsyncWebServerRequest * request) {
    setConfig(request);
  });

  webServer.on("/formatffat", HTTP_POST, [](AsyncWebServerRequest * request) {
    formatFFat(request);
  });

  webServer.on("/rebootInConfigMode", HTTP_POST, [](AsyncWebServerRequest * request) {
    setBootConfigMode(request);
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
    if (sdAvailable && SD.exists(CSV_FILENAME)) {
      DPRINTLN(F("Downloading CSV from SD Card"));
      response = request->beginResponse(SD, CSV_FILENAME, String());
      response->addHeader("Server", "AskSinAnalyzer");
      request->send(response);
    } else {
      DPRINTLN(F("SD Card or CSV file not available"));
      request->send(204, "text/plain", "SD Card or CSV file not available");
    }
  });

  webServer.onNotFound([](AsyncWebServerRequest * request) {
    indexHtml(request);
  });

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  ws.onEvent(onWsEvent);
  webServer.addHandler(&ws);
  webServer.begin();
  MDNS.addService("http", "tcp", 80);
}

bool initmDNS() {
  return MDNS.begin(NetConfig.hostname);
}
#endif
