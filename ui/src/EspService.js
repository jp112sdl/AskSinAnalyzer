import Vue from 'vue';

export default class EspService {

  data = {
    telegrams: [],
    devices: [],
    espConfig: {},
    errors: [],
    errorCnt: 0,
    currentVersion: null,
    latestVersion: null,
    devlistCreated: null,
    rssiLog: [],
  };
  devlist = [];
  maxTelegrams = 100;
  refreshInterval = 2;
  refreshTimeout = null;
  resolveNames = true;
  rssiLogMap = new Map();
  devicesSet = new Set(['==Unbekannt==']);
  ignoreUnknownDevices = false;


  constructor(baseUrl = '', maxTelegrams = 20000, refreshInterval = 2, resolveNames = true, ignoreUnknownDevices = false) {
    this.baseUrl = baseUrl;
    this.maxTelegrams = maxTelegrams;
    this.refreshInterval = refreshInterval;
    this.resolveNames = resolveNames;
    this.ignoreUnknownDevices = ignoreUnknownDevices;
    this.fetchRssiLog();
  }

  addTelegrams(telegrams) {
    telegrams.forEach(t => t.flags = t.flags.split(' ').sort());

    // Add telegrams
    this.data.telegrams.splice(0, 0, ...telegrams);

    // Cap collection
    if (this.data.telegrams.length > this.maxTelegrams) {
      this.data.telegrams.splice(this.maxTelegrams, this.data.telegrams.length - this.maxTelegrams);
    }

    telegrams.forEach(telegram => this.generateDeviceList(telegram));
  }

  async autorefresh() {
    try {
      const lastLognumber = this.data.telegrams[0] ? this.data.telegrams[0].lognumber : -1;
      let telegrams = await this.fetchLog(lastLognumber);
      // Quickly get more telegrams if result holds 50 (max return from esp)
      const refreshInterval = telegrams.length === 50 ? 0 : this.refreshInterval * 1000;
      this.data.errors = [];
      this.data.errorCnt = 0;
      if (telegrams.length) this.addTelegrams(telegrams);
      this.refreshTimeout = setTimeout(() => this.autorefresh(), refreshInterval)
    }
    catch (err) {
      const msg = `API Error getLogByLogNumber: ${ err.message }`;
      if (!this.data.errors.includes(msg)) this.data.errors.unshift(msg);
      this.data.errorCnt++;
      if (this.data.errorCnt < 5) {
        this.refreshTimeout = setTimeout(() => this.autorefresh(), this.refreshInterval * 1000);
      } else {
        this.data.errors.unshift('Too many errors, telegram fetching stopped. Reload App to retry.')
      }
      console.error(err);
    }
  }

  stopAutorefresh() {
    clearTimeout(this.refreshTimeout);
  }

  // Generate unique devices list
  generateDeviceList(telegram = null) {
    let telegrams = [telegram];
    if (telegram === null) {
      this.devicesSet = new Set(['==Unbekannt==']);
      telegrams = this.data.telegrams;
    }
    let newDeviceAdded = false;
    telegrams.forEach(({ fromName, toName, to, from }) => {
      if (fromName && !this.devicesSet.has(fromName)) {
        this.devicesSet.add(fromName);
        newDeviceAdded = true;
      } else if (!fromName && !this.devicesSet.has(from)) {
        this.devicesSet.add(from);
        newDeviceAdded = true;
      }
      if (toName && !this.devicesSet.has(toName)) {
        this.devicesSet.add(toName);
        newDeviceAdded = true;
      } else if (!toName && !this.devicesSet.has(to)) {
        this.devicesSet.add(to);
        newDeviceAdded = true;
      }
    });
    if (newDeviceAdded) {
      const devices = Array.from(this.devicesSet);
      devices.sort((a, b) => a.toString().toLowerCase().localeCompare(b.toString().toLowerCase()));
      this.data.devices.splice(0, devices.length, ...devices);
    }
  }

  async fetchLog(offset = 0) {
    const res = await this._fetch(`${ this.baseUrl }/getLogByLogNumber?format=csv&lognum=${ offset }`);
    let text = await res.text();
    let json = text
      .split(/\r?\n/)
      .map(line => this.csvToObj(line))
      .filter(obj => obj !== null)
      .sort((a,b) => b.lognumber - a.lognumber);
    if (this.resolveNames) {
      json.forEach(t => this.addNameFromDevlist(t));
    }
    if(this.ignoreUnknownDevices) {
      json = json.filter(telegram => telegram.fromNameResolved);
    }
    return json;
  }

  async fetchRssiLog(tstamp = 0) {
    const res = await this._fetch(`${ this.baseUrl }/getRSSILog?fromTstamp=${ tstamp }`);
    (await res.json())
      .filter(item => item.type === 0)
      .forEach(item => {
        if(!this.rssiLogMap.has(item.tstamp * 1000)) {
          this.rssiLogMap.set(item.tstamp * 1000, item.rssi)
        }
      });
    this.data.rssiLog = Array.from(this.rssiLogMap).sort();
    setTimeout(this.fetchRssiLog.bind(this), 2000);
  }

  async fetchConfig() {
    try {
      const res = await this._fetch(`${ this.baseUrl }/getConfig`);
      const espConfig = await res.json();
      espConfig.latestVersion = null; // init reactivity
      espConfig.updateAvailable = false; // init reactivity
      if (espConfig.version_upper) {
        espConfig.currentVersion = espConfig.version_upper.toString().trim()
          + '.'
          + espConfig.version_lower.toString().trim();
      } else {
        espConfig.currentVersion = '0.1';
      }

      // Object.assign(this.data.espConfig, espConfig);
      // Object.keys(espConfig).forEach(key => Vue.set(this.data.espConfig, key, espConfig[key]));
      this.data.espConfig = espConfig;
      return espConfig;
    }
    catch (err) {
      err.message = `API Error getConfig: ${ err.message }`;
      this.data.errors.unshift(err.message);
      this.data.errorCnt++;
      throw err;
    }
  }

  async fetchVersion() {
    try {
      const res = await fetch((process.env.VUE_APP_CDN_URL || 'https://raw.githubusercontent.com/jp112sdl/AskSinAnalyzer/gh-pages/dev') + '/esp-version.txt', { cache: "no-store" });
      if (res.ok) {
        this.data.espConfig.latestVersion = (await res.text()).trim();
        this.data.espConfig.updateAvailable = this.isUpdateAvailable();
      } else {
        console.error(new Error(`${ res.status }: ${ res.statusText }`));
      }
    }
    catch (e) {
      e.message = `Network error while fetching ESP-Version from Github; ${ e.message }`;
      console.error(e);
    }
  }

  async postCommand(cmd) {
    return this._fetch(`${ this.baseUrl }/${ cmd }`, { method: 'post' });
  }

  async ping() {
    try {
      await this._fetch(this.baseUrl + '/index.html'); // TODO: better endpoint?
      return true;
    }
    catch (e) {
      return false;
    }
  }

  async _fetch(opts, init = {}) {
    try {
      const res = await fetch(opts, init);
      if (!res.ok) {
        const err = new Error(`${ res.status }: ${ res.statusText }`);
        err.response = res;
        // noinspection ExceptionCaughtLocallyJS
        throw err;
      }
      return res;
    }
    catch (err) {
      if (!err.response) err.message = 'Network Error! Verify Analyzer IP';
      throw err;
    }
  }

  addNameFromDevlist(telegram) {
    const fromResolved = this.resolveNameFromDevList(telegram.from);
    telegram.fromNameResolved = fromResolved !== null;
    if (fromResolved) {
      telegram.fromName = fromResolved.name;
      telegram.fromSerial = fromResolved.serial;
      telegram.fromIsIp = fromResolved.isIp;
    }
    const toResolved = this.resolveNameFromDevList(telegram.to);
    telegram.toNameResolved = toResolved !== null;
    if (toResolved) {
      telegram.toName = toResolved.name;
      telegram.toSerial = toResolved.serial;
      telegram.toIsIp = toResolved.isIp;
    }
  }

  resolveNameFromDevList(val) {
    const dev = this.devlist.devices.find(({ address }) => address === parseInt(val, 16));
    if (dev) {
      // HmIP SN: 14 chars; HmRF: 10 chars
      return {
        name: dev.name,
        serial: dev.serial,
        isIp: dev.serial.length === 14 || dev.serial === 'HmIP-RF',
      };
    }
    return null;
  }

  csvToObj(strLine) {
    if(strLine.length < 3) return null;
    const splt = strLine.split(';');
    if(splt.length < 9) return null;
    return {
      lognumber: +splt[0],
      tstamp: +splt[1],
      rssi: +splt[2],
      from: splt[3],
      to: splt[4],
      len: +splt[5],
      cnt: +splt[6],
      typ: splt[7],
      flags: splt[8],
    }
  }

  async fetchDevList() {
    try {
      const res = await this._fetch(`${ this.baseUrl }/getAskSinAnalyzerDevListJSON`, { cache: 'reload' });
      const blob = await res.blob();
      let resCharset = 'utf-8';
      try {
        resCharset = res.headers.get('Content-Type').match(/charset=(.+)$/)[1];
        console.log('Use Charset', resCharset, 'for DevList');
      } catch (e) {
        console.warn('Could not read charset from DevList response, using', resCharset);
      }
      const filereader = new FileReader();
      const readed = new Promise(resolve => filereader.addEventListener('loadend', () => resolve(filereader.result)));
      filereader.readAsText(blob, resCharset);
      const data = await readed;
      this.devlist = JSON.parse(data);
      this.data.devlistCreated = this.devlist.created_at;

      if(this.resolveNames) {
        this.data.telegrams.forEach(t => this.addNameFromDevlist(t));
      }
    }
    catch (e) {
      console.error(e);
      this.data.errors.push(`Could not fetch DeviceList from CCU. ${ e.message }`);
    }
    return null;
  }


  isUpdateAvailable() {
    const { latestVersion, currentVersion } = this.data.espConfig;
    if (!latestVersion || !currentVersion) return false;
    const [aU, aL] = latestVersion.split('.');
    const [bU, bL] = currentVersion.split('.');
    return aU > bU || aU === bU && aL > bL;
  }

  execUpdate() {
    const file = `AskSinAnalyzerESP32${ this.data.espConfig.display ? '' : '-ND' }.bin`;
    const { latestVersion, currentVersion } = this.data.espConfig;
    if (!currentVersion) return false;
    const [bU, bL] = currentVersion.split('.');
    if (bU >= 3 && bL >=6) {
      document.location.href = `${ this.baseUrl }/update`;
    } else {
      document.location.href = `${ this.baseUrl }/httpupdate?url=https://raw.githubusercontent.com/jp112sdl/AskSinAnalyzer/master/ota/${ file }`;
    }
  }

  async insertSD() {
    const res = await this._fetch(`${ this.baseUrl }/insertSD`);
    const txt = await res.text();
    if (txt !== 'OK') {
      throw new Error(txt);
    }
    await this.fetchConfig()
  }

  async ejectSD() {
    const res = await this._fetch(`${ this.baseUrl }/ejectSD`);
    const txt = await res.text();
    if (txt !== 'OK') {
      throw new Error(txt);
    }
    await this.fetchConfig()
  }
}
