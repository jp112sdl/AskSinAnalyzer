#ifndef CCUFUNCTIONS_H_
#define CCUFUNCTIONS_H_


//http://CCU_IP:8181/cuxd.exe?ret=dom.GetObject((xmlrpc.GetObjectByHSSAddress(interfaces.Get("BidCos-RF"),"<SERIAL>:0")).Name())

bool setCCURequest(String val) {
  if (WiFi.status() == WL_CONNECTED) {
    #ifdef USE_DISPLAY
    drawStatusCircle(ILI9341_BLUE);
    #endif
    HTTPClient http;
    //http.setTimeout(HTTPTimeOut);
    String url = "http://" + String(HomeMaticConfig.ccuIP) + ":8181/a.exe?ret=dom.GetObject(%22" + String(HomeMaticConfig.SVAnalyzeInput) + "%22).State(%22" + val + "%22)";
    DPRINTLN("setCCURequest url: " + url);
    http.begin(url);
    int httpCode = http.GET();
    String payload = "";

    if (httpCode > 0) {
      DPRINTLN("HTTP success");
      payload = http.getString();
    }
    if (httpCode != 200) {
      DPRINTLN("HTTP failed with HTTP Error Code " + String(httpCode));
    }
    http.end();

    payload = payload.substring(payload.indexOf("<ret>"));
    payload = payload.substring(5, payload.indexOf("</ret>"));

    DPRINTLN("result: " + payload);
    return (payload != "null");
  } else return false;
}

String getCCURequestResult() {
  if (WiFi.status() == WL_CONNECTED) {
    #ifdef USE_DISPLAY
    drawStatusCircle(ILI9341_BLUE);
    #endif
    HTTPClient http;
    //http.setTimeout(HTTPTimeOut);
    String url = "http://" + String(HomeMaticConfig.ccuIP) + ":8181/a.exe?ret=dom.GetObject(%22" + String(HomeMaticConfig.SVAnalyzeOutput) + "%22).State()";
    DPRINTLN("getCCURequestResult url: " + url);
    http.begin(url);
    int httpCode = http.GET();
    String payload = "error";
    if (httpCode > 0) {
      payload = http.getString();
    }
    if (httpCode != 200) {
      DPRINTLN("HTTP fail");
    }
    http.end();

    payload = payload.substring(payload.indexOf("<ret>"));
    payload = payload.substring(5, payload.indexOf("</ret>"));
    DPRINTLN("result: " + payload);

    return payload;
  } else {
    DPRINTLN("getCCURequestResult: WiFi.status() != WL_CONNECTED, trying to reconnect");
    return "ERR";
  }
}

#endif
