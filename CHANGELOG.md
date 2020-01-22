## Version 3.1
- **AskSinSniffer328P:**<br/>
⚠️ um alle neuen Features (RSSI-Anzeige) nutzen zu können, ist auch ein erneutes Flashen des [AskSinSniffer328P](https://github.com/jp112sdl/AskSinAnalyzer/tree/master/AskSinSniffer328P) notwendig!<br/>
Der Code läuft mit dem aktuellen [AskSinPP-master](https://github.com/pa-pa/AskSinPP) Branch.
  - zyklische Ausgabe des Empfangspegels (RSSI) alle 750ms hinzugefügt
- **ESP32:**
  - **neu:** RSSI Anzeige einzeln 
    - Text+Bargraph des aktuellen Pegels, 
    - Peak(-Hold) Anzeige, 
    - Quelle des Pegels
  - **neu:** RSSI Anzeige Bargraph
  - farbliche Unterscheidung der Pegel:
    - gelb = Störpegel
    - grün = Empfangsstärke eines empfangenden HM RF Telegramms
    - hellblau = Empfangsstärke eines empfangenden HmIP Telegramms
  - **Schalter an GPIO 4** schaltet zwischen 2 RSSI Modi:
    - _EIN_ = es wird nur der Wert des Störpegels _("Grundrauschen")_ angezeigt
    - _AUS_ = es werden Störpegel und die Empfangsstärke von Telegrammen angezeigt
  - kurzer Druck am Taster schaltet zwischen den Anzeigen
    - Telegrammauswertung (Startseite)
    - RSSI Anzeige einzeln
    - RSSI Anzeige Balkendiagramm
    - Info-Seite
  - CSV-Datei:  
    - Spalte "msg" hinzugefügt, sie enthält das komplette Telegramm
  - AP-Modus:
    - Auswahl des Backends `CCU` oder `Other` (z.B. für FHEM) hinzugefügt
    - Eingabe der CCU-IP (`CCU`) oder Backend-URL (`Other`)
  - Codeoptimierungen (thx to [pa-pa](https://github.com/pa-pa))  
- **WebUI:**
  - Unterstützung weiterer Zeichenkodierungen

## Version 2.7
- **ESP32:**
  - Behebung von Stabilitätsproblemen
    - Fix: es wird nicht mehr versucht, die CCU zu erreichen, wenn die Adressauflösung deaktiviert ist
    - Fix: die Systemvariable "AskSinAnalyzerDevList" wird explizit als ID_SYSTEM_VARIABLES abgerufen

## Version 2.6
- **ESP32:**
  - Behebung von Stabilitätsproblemen
  - kompiliert mit neuester ESP32 Board-Unterstützung 1.0.4
  - ESPAsyncWebServer und AsyncTCP aktualisiert

## Version 2.5
- **ESP32:**
  - Uhrzeit korrigiert (UTC für WebUI)
  
## Version 2.4
- **ESP32:**
  - Berechnung der Sommerzeit korrigiert
  - serieller Eingangspuffer auf 2k erhöht
  - bei Verwendung einer statischen IP wird das Gateway auch als DNS Serveradresse genutzt
  
## Version 2.3
- **ESP32:**
  - das Speichern der Systemkonfigurationsdaten (CCU IP, Hostname, NTP etc) erfolgt ab sofort im NVS *und nicht mehr mithilfe einer Datei im SPIFFS*
    - ❗ **nach dem Einspielen dieser Version müssen die Konfigurationsdaten einmalig erneut eingegeben werden**
  - Begrenzung der lokalen Logdatei auf die letzten 1000 Einträge aufgrund von Stabilitätsproblemen   

## Version 2.2
- **ESP32:** 
  - Loggen in die CSV Datei erfolgt nur noch auf SD-Karte
    - bei Geräten ohne SD Kartenleser ist keine CSV-Datei mehr herunterladbar
  - fixed [#34](https://github.com/jp112sdl/AskSinAnalyzer/issues/34)
  - Einführung von WebSocket
    - an `/ws` können sich bis zu 3 WebSocket-Clients verbinden und Daten im JSON-Format (jeweils 1 Zeile pro Telegramm) empfangen, z.B.: 
    ```json
    {
      "lognumber": 305, 
      "tstamp": 1566241748, 
      "rssi": -88, 
      "from": "098765", 
      "to": "102938", 
      "len": 26, 
      "cnt": 210, 
      "typ": "WEATHER", 
      "flags": "BCAST RPTEN"
    }
    ```
- **WebUI:** 
  - Begrenzung der letzten 50 Telegramme beim initialen Laden der Webseite aufgehoben
    - die aktuelle Sitzung wird nun in einer lokalen Logdatei (im SPIFFS) des ESP32 gespeichert
    - die WebUI lädt alle Telegramme, die seit Start aufgezeichnet wurden aus der Datei und zeigt sie an
  - Import von CSV Dateien
    - eine heruntergeladene CSV Datei kann jederzeit importiert und dessen Inhalt angezeigt werden
    - wird eine CSV importiert, stoppt der Live-Abruf vom ESP32

## Version 2.0 / 2.1
- **ESP32**:
  - Veränderung der Abfrage der Geräteliste von der CCU
    - die bisherigen Systemvariablen `SV_Analyze_Input` und `SV_Analyze_Output` sowie das zugehörige Programm können gelöscht werden
    - ❗️ **es muss nun ein Skript ausgeführt werden ➡️ siehe [Wiki](https://github.com/jp112sdl/AskSinAnalyzer/wiki/CCU_Unterstützung)**
  
- **ESP32 / WebUI**: 
  - Unterstützung der *teilweisen* Dekodierung von HmIP_Funktelegrammen
    - Sender und Empfänger können angezeigt werden
    - *eine dedizierte Einordnung der Telegramm-Typen/-Flags ist nicht möglich*
