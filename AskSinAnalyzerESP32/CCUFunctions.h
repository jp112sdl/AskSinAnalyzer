//- -----------------------------------------------------------------------------------------------------------------------
// AskSinAnalyzer
// 2019-06-01 jp112sdl Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------


#ifndef CCUFUNCTIONS_H_
#define CCUFUNCTIONS_H_


//http://CCU_IP:8181/cuxd.exe?ret=dom.GetObject((xmlrpc.GetObjectByHSSAddress(interfaces.Get("BidCos-RF"),"<SERIAL>:0")).Name())

String loadAskSinAnalyzerDevListFromCCU() {
  if (isOnline && WiFi.status() == WL_CONNECTED) {
    DPRINTLN(F("- Loading DevList from CCU... "));
#ifdef USE_DISPLAY
    drawStatusCircle(ILI9341_BLUE);
#endif
    HTTPClient http;
    //http.setTimeout(HTTPTimeOut);
    String url = "http://" + String(HomeMaticConfig.ccuIP) + ":8181/a.exe?ret=dom.GetObject(%22" + CCU_SV + "%22).Value()";
    //DPRINTLN("loadAskSinAnalyzerDevListFromCCU url: " + url);
    http.begin(url);
    int httpCode = http.GET();
    String payload = "ERROR";
    if (httpCode > 0) {
      payload = http.getString();
    }
    if (httpCode != 200) {
      DPRINT("HTTP failed with code "); DDECLN(httpCode);
    }
    http.end();

    payload = payload.substring(payload.indexOf("<ret>"));
    payload = payload.substring(5, payload.indexOf("</ret>"));
    payload.replace("&quot;", "\"");
    //DPRINTLN("result: " + payload);
#ifdef USE_DISPLAY
    drawStatusCircle(ILI9341_GREEN);
#endif
    return payload;
  }

  DPRINTLN("loadAskSinAnalyzerDevListFromCCU: ERROR");
#ifdef USE_DISPLAY
  drawStatusCircle(ILI9341_RED);
#endif
  return "ERROR";
}

#endif
