#ifndef __WEB__H_
#define __WEB__H_

#include "Web_JS.h"
#include "Web_CSS.h"
#include "Web_HTML.h"

AsyncWebServer webServer(80);

void getLog(AsyncWebServerRequest *request) {

  uint16_t start = 0;
  if (request->hasParam("start")) {
    AsyncWebParameter* p = request->getParam("start");
    start =  p->value().toInt();
  }
  uint16_t jsonlogLength = ((start) <= logLength) ? logLength - start : logLength;

  String json = "{\"loglength\":\"" + String(logLength) + "\", \"logentries\": [";

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

  webServer.onNotFound([](AsyncWebServerRequest * request) {
    defaultHtml(request);
  });

  webServer.begin();
}

#endif
