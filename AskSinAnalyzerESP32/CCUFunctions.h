#ifndef CCUFUNCTIONS_H_
#define CCUFUNCTIONS_H_

bool setCCURequest(String SV, String val) {
  if (WiFi.status() == WL_CONNECTED) {
    drawStatusCircle(ILI9341_BLUE);
    HTTPClient http;
    //http.setTimeout(HTTPTimeOut);
    String url = "http://" + CCU_IP + ":8181/a.exe?ret=dom.GetObject(%22" + SV + "%22).State(%22" + val + "%22)";
    Serial.println("setCCURequest url: " + url);
    http.begin(url);
    int httpCode = http.GET();
    String payload = "";

    if (httpCode > 0) {
      Serial.println("HTTP success");
      payload = http.getString();
    }
    if (httpCode != 200) {
      Serial.println("HTTP failed with HTTP Error Code " + String(httpCode));
    }
    http.end();

    payload = payload.substring(payload.indexOf("<ret>"));
    payload = payload.substring(5, payload.indexOf("</ret>"));

    Serial.println("result: " + payload);
    return (payload != "null");
  } else return false;
}

String getCCURequestResult(String SV) {
  if (WiFi.status() == WL_CONNECTED) {
    drawStatusCircle(ILI9341_BLUE);
    HTTPClient http;
    //http.setTimeout(HTTPTimeOut);
    String url = "http://" + CCU_IP + ":8181/a.exe?ret=dom.GetObject(%22" + SV + "%22).State()";
    Serial.println("getCCURequestResult url: " + url);
    http.begin(url);
    int httpCode = http.GET();
    String payload = "error";
    if (httpCode > 0) {
      payload = http.getString();
    }
    if (httpCode != 200) {
      Serial.println("HTTP fail");
    }
    http.end();

    payload = payload.substring(payload.indexOf("<ret>"));
    payload = payload.substring(5, payload.indexOf("</ret>"));
    Serial.println("result: " + payload);

    return payload;
  } else {
    Serial.println("getCCURequestResult: WiFi.status() != WL_CONNECTED, trying to reconnect");
    return "ERR";
  }
}

#endif
