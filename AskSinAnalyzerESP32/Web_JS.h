//- -----------------------------------------------------------------------------------------------------------------------
// AskSinAnalyzer
// 2019-06-01 jp112sdl Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------


const char HTTP_JS[] PROGMEM = R"=====(
<script>
  var loglen = 0;
  var first = 1;
  var cnt = 0;

  function Get(u){ 
    var h = new XMLHttpRequest(); 
    h.open('GET',u,false); 
    h.send(null); 
    return h.responseText; 
  } 

  function refreshState(rekursiv) { 
    var i;
    var json_obj = JSON.parse(Get('/getLog?start='+loglen+'&first='+first)); 
    first = 0;
    loglen = json_obj.loglength;
    
    var table = document.getElementById("logtable");
     for (i in json_obj.logentries) {
       cnt++;
       var row = table.insertRow(1);
       var cellNum = row.insertCell(0);
       var cellTime = row.insertCell(1);
       var cellRSSI = row.insertCell(2);
       var cellFrom = row.insertCell(3);
       var cellTo = row.insertCell(4);
       var cellLen = row.insertCell(5);
       var cellCnt = row.insertCell(6);
       var cellTyp = row.insertCell(7);
       var cellFlags = row.insertCell(8);
       cellNum.innerHTML = cnt;
       cellTime.innerHTML = json_obj.logentries[i].time;
       cellRSSI.innerHTML = json_obj.logentries[i].rssi;
       cellFrom.innerHTML = json_obj.logentries[i].from;
       cellTo.innerHTML = json_obj.logentries[i].to;
       cellLen.innerHTML = json_obj.logentries[i].len;
       cellCnt.innerHTML = json_obj.logentries[i].cnt;
       cellTyp.innerHTML = json_obj.logentries[i].typ;
       cellFlags.innerHTML = json_obj.logentries[i].flags;
     }

    if (rekursiv) setTimeout(function(){ refreshState(true); }, 1000); 
  } 
  
  /*init refresh:*/ 
  refreshState(true); 
</script>
)=====";
