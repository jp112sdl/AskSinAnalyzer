const http = require('http');
const { parse: parseUrl } = require('url');
const { readFileSync } = require('fs');
const devlist = readFileSync('./devlist.xml');
const devlistDevices = require('./devlist.json').devices;

let SNs = devlistDevices.map(({serial}) => serial);
SNs.push('123gibtsnet');
SNs.push('987gibtsnet');

const typs = [
  'INFO',
  'CLIMATECTRL_EVENT',
  'RESPONSE',
  'SENSOR_DATA',
  'POWER_EVENT',
  'ACTION',
];

const flags = ['WKUP', 'WKMEUP', 'BCAST', 'BURST', 'BIDI', 'RPTED', 'RPTEN'];

let lognumber = 0;

function genTelegram() {
  lognumber++;
  return {
    "tstamp": Math.round(Date.now() / 1000),
    "lognumber": lognumber,
    "rssi": -1 * Math.round(Math.random() * 120) - 20,
    "from": SNs[Math.floor(Math.random() * SNs.length)],
    "to": SNs[Math.floor(Math.random() * SNs.length)],
    "len": Math.round(Math.random() * 100),
    "cnt": Math.round(Math.random() * 100),
    "typ": typs[Math.floor(Math.random() * typs.length)],
    "flags": flags.sort(() => Math.random() - 0.5).slice(0, Math.floor(Math.random()*3)+1).join(' ')
  }
}

let data = [];

setInterval(() => {
  const cnt = Math.random() * 7;
  for (let i = 0; i < cnt; i++) {
    data.unshift(genTelegram());
  }
  data = data.slice(0, 200);
}, 1000);


const server = http.createServer(function(req, res) {
  res.setHeader('Access-Control-Allow-Origin', '*');
  const url = parseUrl(req.url, true);
  {
    switch (url.pathname) {

      case '/getAskSinAnalyzerDevList':
        res.setHeader('Content-Type','application/json');
        res.write(devlist);
        res.end();
        break;

      case '/getLogByLogNumber':
        const offset = url.query && url.query.lognum;
        res.write(JSON.stringify(
          data.filter(item => item.lognumber > offset).slice(-50),
          null, 2)
        ); //write a response
        res.end(); //end the response
        break;

      case '/getConfig':
        res.write(JSON.stringify({
            "staticipconfig": 0,
            "ip": "192.168.1.191",
            "netmask": "255.255.255.0",
            "gateway": "192.168.1.1",
            "macaddress": "30:AE:A4:38:88:6C",
            "hostname": "my.hostname.local",
            "version_upper": 1,
            "version_lower": 3,
            "ccuip": "192.168.178.39",
            "svanalyzeinput": "Analyzer_Input",
            "svanalyzeoutput": "Analyzer_Output",
            "resolve": 1,
            "sdcardavailable": 1,
            "sdcardsizemb": 3780,
            "sdcardtotalspacemb": "2047",
            "sdcardusedspacemb": "4",
            "spiffssizekb": 1342,
            "spiffsusedkb": 79,
            "boottime": Math.floor(Date.now() / 1000),
            "ntp": "0.de.pool.ntp.org",
            "display": false,
          }, null, 2)
        );
        res.end();
        break;

      case '/reboot':
      case '/rebootconfig':
        console.log('Simulate ESP reboot, closing listener');
        res.writeHead(200);
        res.end();
        setTimeout(() => server.close(),1000);
        setTimeout(() => server.listen(3000,() => console.log('Listener opened')),10*1000);
        break;

      case '/deletecsv':
      case '/index.html':
        res.write(':)');
        res.end();
        break;

      // case '/getDeviceNameBySerial':
      //   const sn = url.query && url.query.Serial;
      //   const name = devices[sn] || null;
      //   res.write(JSON.stringify(name));
      //   res.end();
      //   break;

      default:
        res.writeHead(404);
        res.write('Not found');
        res.end();
    }
  }
});

server.listen(3000, () => console.log("Server start at port 3000"));
