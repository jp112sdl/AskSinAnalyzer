const http = require('http');
const { parse: parseUrl } = require('url');
const devlist = require('./devlist.json');

let Addrs = devlist.devices.map(({ address }) => address.toString(16));
Addrs.push('123gibtsnet');
Addrs.push('987gibtsnet');

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
    "from": Addrs[Math.floor(Math.random() * Addrs.length)],
    "to": Addrs[Math.floor(Math.random() * Addrs.length)],
    "len": Math.round(Math.random() * 100),
    "cnt": Math.round(Math.random() * 100),
    "typ": typs[Math.floor(Math.random() * typs.length)],
    "flags": flags.sort(() => Math.random() - 0.5).slice(0, Math.floor(Math.random() * 3) + 1).join(' ')
  }
}

const espConfig = {
  "staticipconfig": 0,
  "ip": "192.168.1.191",
  "netmask": "255.255.255.0",
  "gw": "192.168.1.1",
  "macaddress": "30:AE:A4:38:88:6C",
  "hostname": "my.hostname.local",
  "version_upper": 1,
  "version_lower": 3,
  "ccuip": "192.168.178.39",
  "ccuhttps": 0,
  "backendurl": "http://localhost",
  "backend": 0,
  "rssi_hbw": 30,
  "rssi_alarmthreshold": -80,
  "rssi_alarmcount": 5,
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
};

let data = [];
setInterval(() => {
  if (Math.random() < 0.5) return;
  const cnt = Math.random() * 3;
  for (let i = 0; i < cnt; i++) {
    data.unshift(genTelegram());
  }
  data = data.slice(0, 200);
}, 2000);

let rssiLog = [];
setInterval(() => {
  rssiLog.unshift({
    tstamp: Math.round(Date.now() / 1000),
    type: 0,
    rssi: Math.round(Math.random() * 50) - 120,
  });
  rssiLog = rssiLog.slice(0, 500);
}, 750);


function telegram2Csv(obj) {
  return `${ obj.lognumber };${ obj.tstamp };${ obj.rssi };${ obj.from };${ obj.to };${ obj.len };${ obj.cnt };${ obj.typ };${ obj.flags };`
}

const server = http.createServer(function(req, res) {
  res.setHeader('Access-Control-Allow-Origin', '*');
  const url = parseUrl(req.url, true);
  switch (url.pathname) {

    case '/insertSD':
      espConfig.sdcardavailable = 1;
      res.write('OK');
      res.end();
      break;
    case '/ejectSD':
      espConfig.sdcardavailable = 0;
      res.write('OK');
      res.end();
      break;

    case '/fhem':
      res.setHeader('Content-Type', 'application/json charset=utf-8');
      res.write(JSON.stringify(devlist, null, 2));
      res.end();
      break;

    case '/getAskSinAnalyzerDevListJSON':
      res.setHeader('Content-Type', 'application/json charset=iso-8859-1');
      res.write(JSON.stringify(devlist, null, 2));
      res.end();
      break;

    case '/getLogByLogNumber':
      const offset = url.query && url.query.lognum;
      const format = url.query && url.query.format;
      let resData = data.filter(item => item.lognumber > offset).slice(-50);
      if (offset === "-1") resData.reverse();
      if (format === "csv") {
        res.write(resData.map(item => telegram2Csv(item)).join("\n"));
      } else {
        res.write(JSON.stringify(resData, null, 2)); //write a response
      }
      res.end(); //end the response
      break;

    case '/getConfig':
      res.write(JSON.stringify(espConfig, null, 2)
      );
      res.end();
      break;

    case '/getRSSILog':
      res.write(JSON.stringify(rssiLog, null, 2));
      res.end();
      break;

    case '/reboot':
    case '/rebootInConfigMode':
      console.log('Simulate ESP reboot, closing listener');
      res.writeHead(200);
      res.end();
      setTimeout(() => server.close(), 1000);
      setTimeout(() => server.listen(3000, () => console.log('Listener opened')), 10 * 1000);
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
});

server.listen(3000, () => console.log("Server start at port 3000"));
