//- -----------------------------------------------------------------------------------------------------------------------
// AskSinAnalyzer
// 2019-06-01 jp112sdl Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------


const char HTTP_CSS[] PROGMEM = R"=====(
<style>
div {
  white-space: nowrap;
}

input.lnkbtn,
button {
  background-color: #1fa3ec;
}

input[type=text] {
  width: 95%;
}

.green {
  color: green;
}

.red {
  color: red;
}

.black {
  color: black;
}

.tdr {
  text-align: right;
}

.tdl {
  text-align: left;
}

.tdc {
  text-align: center;
}

input.lnkbtn, input.greybtn {
  -webkit-appearance: button;
  -moz-appearance: button;
  appearance: button;
}

body {
  background-color: #eee;
}

input.lnkbtn,
input.greybtn,
button {
  cursor: pointer;
  color: #fff;
  line-height: 2.4rem;
  font-size: 1.2rem;
  width: 100%;
  padding: 5px;
}

input,
button,
input.lnkbtn,
input.greybtn {
  border: 0;
  border-radius: 0.3rem;
}

.c {
  text-align: center;
}

.k {
  font-style: italic;
}

div,
input {
  padding: 5px;
  font-size: 1em;
}

body {
  text-align: center;
  font-family: helvetica;
  font-size: 100%;
}

.q {
  float: right;
  width: 64px;
  text-align: right;
}

.ls {
  font-weight: bold;
  text-align: center;
  font-size: 300%;
}

.lt {
  font-size: 150%;
  text-align: center;
}

.redbtn {
  background-color: #ff0000;
} 

.yellowbtn {
  background-color: #f47d42;
} 

input.greybtn {
  background-color: #777;
}
  
table {
  width: 100%;
}

td {
  max-width: 100%;
  font-weight: bold;
}

.disabled {
  -webkit-filter: blur(1px);
  opacity:0.2;
}
</style>
)=====";




