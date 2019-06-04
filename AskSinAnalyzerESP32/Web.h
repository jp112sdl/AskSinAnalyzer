#ifndef __WEB__H_
#define __WEB__H_

#include "Web_JS.h"
#include "Web_CSS.h"
#include "Web_HTML.h"

AsyncWebServer webServer(80);

void getConfig (AsyncWebServerRequest *request) {
  String json = "{";
  json += "\"staticip\":\"" + String(NetConfig.ip) + "\"";
  json += ",";
  json += "\"staticnetmask\":\"" + String(NetConfig.netmask) + "\"";
  json += ",";
  json += "\"staticgateway\":\"" + String(NetConfig.gw) + "\"";
  json += ",";
  json += "\"ccuip\":\"" + String(HomeMaticConfig.ccuIP) + "\"";
  json += ",";
  json += "\"svanalyzeinput\":\"" + String(HomeMaticConfig.SVAnalyzeInput) + "\"";
  json += ",";
  json += "\"svanalyzeoutput\":\"" + String(HomeMaticConfig.SVAnalyzeOutput) + "\"";
  json += ",";
  json += "\"resolve\":\"" + String(RESOLVE_ADDRESS) + "\"";
  json += "}";

  AsyncWebServerResponse *response = request->beginResponse(200);
  response->addHeader("Content-Length", String(json.length()));
  request->send(200, "text/json", json);
}


void getLogByLogNumber (AsyncWebServerRequest *request) {
  uint32_t lognum = 0;
  if (request->hasParam("lognum")) {
    AsyncWebParameter* p = request->getParam("lognum");
    lognum = p->value().toInt();
  }

  String json = "[";

  for (uint16_t l = 0; l < logLength; l++) {
    if (LogTable[l].lognumber > lognum && l < 200) {
      json += "{";
      json += "\"lognumber\": " + String(LogTable[l].lognumber) + ", ";
      json += "\"tstamp\": " + String(LogTable[l].time) + ", ";
      json += "\"rssi\": " + String(LogTable[l].rssi) + ", ";
      String from = String(LogTable[l].from);
      from.trim();
      json += "\"from\": \"" + from + "\", ";
      String to = String(LogTable[l].to);
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
      json += ",";
    }
    if (l == 200) break;
  }

  json += "]";
  json.replace("},]", "}]");

  AsyncWebServerResponse *response = request->beginResponse(200);
  response->addHeader("Content-Length", String(json.length()));
  request->send(200, "text/json", json);
}

void getLogByTimestamp (AsyncWebServerRequest *request) {
  time_t ts = 0;
  if (request->hasParam("ts")) {
    AsyncWebParameter* p = request->getParam("ts");
    ts = strtol(p->value().c_str(), 0, 10);
  }

  String json = "[";

  for (uint16_t l = 0; l < logLength; l++) {
    if (LogTable[l].time > ts && l < 200) {
      json += "{";
      json += "\"tstamp\": " + String(LogTable[l].time) + ", ";
      json += "\"rssi\": " + String(LogTable[l].rssi) + ", ";
      String from = String(LogTable[l].from);
      from.trim();
      json += "\"from\": \"" + from + "\", ";
      String to = String(LogTable[l].to);
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
      json += ",";
    }
    if (l == 200) break;
  }

  json += "]";
  json.replace("},]", "}]");

  AsyncWebServerResponse *response = request->beginResponse(200);
  response->addHeader("Content-Length", String(json.length()));
  request->send(200, "text/json", json);
}

void getLog(AsyncWebServerRequest *request) {

  uint16_t start = 0;
  bool first = false;

  if (request->hasParam("first")) {
    AsyncWebParameter* p = request->getParam("first");
    first = (p->value() == "1");
  }

  if (request->hasParam("start")) {
    AsyncWebParameter* p = request->getParam("start");
    start = (first == true) ? allCount - 200 : p->value().toInt();
  }

  uint16_t jsonlogLength = ((start) <= allCount) ? allCount - start : allCount;

  String json = "{\"loglength\":\"" + String(allCount) + "\", \"logentries\": [";

  if (jsonlogLength > 0) {

    for (int16_t l = jsonlogLength - 1; l >= 0; l--) {
      json += "{";
      json += "\"time\": \"" + getDatum(LogTable[l].time) + " " + getUhrzeit(LogTable[l].time) + "\", ";
      json += "\"rssi\": \"" + String(LogTable[l].rssi) + "\", ";
      json += "\"from\": \"" + String(LogTable[l].from) + "\", ";
      json += "\"to\": \"" + String(LogTable[l].to) + "\", ";
      json += "\"len\": \"" + String(LogTable[l].len) + "\", ";
      json += "\"cnt\": \"" + String(LogTable[l].cnt) + "\", ";
      String t = String(LogTable[l].typ);
      t.trim();
      json += "\"typ\": \"" + t + "\", ";
      String fl = String(LogTable[l].flags);
      fl.trim();
      json += "\"flags\": \"" + fl + "\"";
      json += "}";
      if (l > 0) json += ",";
    }
  }
  json += "]";

  json += "}";
  request->send(200, "text/json", json);
}


void defaultHtml(AsyncWebServerRequest *request) {
  String page = "";

  page = FPSTR(HTTP_DEFAULT);

  page.replace("{css_style}", FPSTR(HTTP_CSS));
  page.replace("{js}", FPSTR(HTTP_JS));

  AsyncWebServerResponse *response = request->beginResponse(200);
  response->addHeader("Content-Length", String(page.length()));
  request->send(200, "text/html", page);
}

void indexHtml(AsyncWebServerRequest *request) {
  String page = FPSTR(HTTP_INDEX);

  AsyncWebServerResponse *response = request->beginResponse(200);
  response->addHeader("Content-Length", String(page.length()));
  request->send(200, "text/html", page);
}

void setBootConfigMode(AsyncWebServerRequest *request) {
  if (SPIFFS.begin()) {
    Serial.println(F("setBootConfigMode mounted file system"));
    if (!SPIFFS.exists(BOOTCONFIGMODE_FILENAME)) {
      File bootConfigModeFile = SPIFFS.open(BOOTCONFIGMODE_FILENAME, "w");
      bootConfigModeFile.print("0");
      bootConfigModeFile.close();
      SPIFFS.end();
      Serial.println(F("Boot to ConfigMode requested. Restarting..."));
      request->send(200, "text/plain", F("<state>enableBootConfigMode - Rebooting</state>"));
      delay(500);
      ESP.restart();
    } else {
      request->send(200, "text/plain", F("<state>enableBootConfigMode - FAILED!</state>"));
      SPIFFS.end();
    }
  }
}

void initWebServer() {
  webServer.on("/restart", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", "rebooting");
    delay(100);
    ESP.restart();
  });

  webServer.on("/getLog", HTTP_GET, [](AsyncWebServerRequest * request) {
    getLog(request);
  });

  webServer.on("/getConfig", HTTP_GET, [](AsyncWebServerRequest * request) {
    getConfig(request);
  });

  webServer.on("/rebootconfig", HTTP_GET, [](AsyncWebServerRequest * request) {
    setBootConfigMode(request);
  });

  webServer.on("/getLogByTimestamp", HTTP_GET, [](AsyncWebServerRequest * request) {
    getLogByTimestamp(request);
  });

  webServer.on("/getLogByLogNumber", HTTP_GET, [](AsyncWebServerRequest * request) {
    getLogByLogNumber(request);
  });

  webServer.on("/deletecsv", HTTP_GET, [](AsyncWebServerRequest * request) {
    bool backup = false;
    if (request->hasParam("backup")) {
      AsyncWebParameter* p = request->getParam("backup");
      backup = (p->value() == "1");
    }
    deleteCSV(CSV_FILENAME, backup);
    request->send(200, "text/plain", "csv deleted, " + (String)((backup == true) ? "with" : "without") + " backup");
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

  webServer.begin();
}

#endif
