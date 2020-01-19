//- -----------------------------------------------------------------------------------------------------------------------
// AskSinAnalyzer
// 2019-06-01 jp112sdl Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------

#ifndef WM_JS_H_
#define WM_JS_H_


const char WM_HTTP_SCRIPT[] PROGMEM = R"=====(
<script>
function setBackendType() {
    var qsa = document.querySelectorAll('[id^=div_]'); 
    qsa.forEach(function(e) { 
      e.style.display = 'block'; 
    }); 
      
    if (backendtype) { 
      var flt; 
      switch (parseInt(backendtype.value)) { 
        case 0: 
          flt = 'backendurl'; 
          break; 
        case 1: 
          flt = 'ccu'; 
          break; 
      } 
      qsa = document.querySelectorAll('[id^=div_' + flt + ']'); 
      qsa.forEach(function(e) { e.style.display = 'none'; }); 
    } 
}
</script>
)=====";

#endif /* WM_JS_H_ */
