//- -----------------------------------------------------------------------------------------------------------------------
// AskSinAnalyzer
// 2019-06-01 jp112sdl Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------


#ifndef CCUFUNCTIONS_H_
#define CCUFUNCTIONS_H_


//http://CCU_IP:8181/cuxd.exe?ret=dom.GetObject((xmlrpc.GetObjectByHSSAddress(interfaces.Get("BidCos-RF"),"<SERIAL>:0")).Name())

String getCCURequest(String req) {
  if (WiFi.status() == WL_CONNECTED) {
#ifdef USE_DISPLAY
    drawStatusCircle(ILI9341_BLUE);
#endif
    HTTPClient http;
    //http.setTimeout(HTTPTimeOut);
    String url = "http://" + String(HomeMaticConfig.ccuIP) + ":8181/a.exe?ret=dom.GetObject(%22" + req + "%22).Value()";
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
    //DPRINTLN("result: " + payload);

    return payload;
  } else {
    DPRINTLN("getCCURequestResult: WiFi.status() != WL_CONNECTED, trying to reconnect");
    return "ERR";
  }
}

#endif
