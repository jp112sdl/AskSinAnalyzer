#ifndef __WEB__H_
#define __WEB__H_

#include "Web_JS.h"
#include "Web_CSS.h"
#include "Web_HTML.h"

AsyncWebServer webServer(80);

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

void initWebServer() {
  webServer.on("/restart", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", "rebooting");
    delay(100);
    ESP.restart();
  });

  webServer.on("/getLog", HTTP_GET, [](AsyncWebServerRequest * request) {
    getLog(request);
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
    defaultHtml(request);
  });

  webServer.begin();
}

#endif
