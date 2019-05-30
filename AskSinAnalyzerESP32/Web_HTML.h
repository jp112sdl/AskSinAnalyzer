//
// 2018-11-22 jp112sdl Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//

const char HTTP_DEFAULT[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no" />
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
    <title>AskSinAnalyzer</title>
    {css_style}
  </head>
  <body>
    <div> 
    <table id='logtable' class='tdc' >
    <tr><td>Uhrzeit</td><td>RSSI</td><td>FROM</td><td>TO</td><td>LEN</td><td>CNT</td><td>TYP</td><td>FLAGS</td>
    </tr>
    </table>
    </div> 
    {js}
  </body>
</html>
)=====";

