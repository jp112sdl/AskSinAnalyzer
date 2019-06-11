//- -----------------------------------------------------------------------------------------------------------------------
// AskSinAnalyzer
// 2019-06-01 jp112sdl Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------

const char HTTP_STYLE[] PROGMEM = R"=====(
<style>
  .c {
    text-align: center;
  } 
  
  div,input{
    padding:5px;
    font-size:1em;
  } 
  
  input[type=text],input[type=password]{
    width:95%;
  } 
  
  body{
    text-align: center;
    font-family:verdana;
  } 
  
  button{
    border:0;
    border-radius:0.3rem;
    background-color:#1fa3ed;
    color:#fff;
    line-height:2.4rem;
    font-size:1.2rem;
    width:100%;
  } 
  
  .ckb{
    float: right;
    width: 18px;
    text-align: right;
  } 
  
  .cob { 
    border: 1px solid #ccc; 
    width: 180px; 
    border-radius: 3px; 
    overflow: hidden; 
    background: #fafafa no-repeat 90% 50%;
  } 
  
  .cob select { 
    padding: 5px 8px; 
    border: none; 
    box-shadow: none; 
    background: transparent; 
    background-image: none; 
    webkit-appearance: none; 
    font-size: 1em;
  }
  
  .cob select:focus { 
    outline: none;
  } 
  
  .q {
    float: right;
    width: 64px;
    text-align: right;
  } 
  .l{
    background: url("data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==") no-repeat left center;
    background-size: 1em;
  }
</style>
)=====";

