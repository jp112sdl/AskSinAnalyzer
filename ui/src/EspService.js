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
  };
  devlist = [];
  maxTelegrams = 100;
  refreshInterval = 2;

  constructor(baseUrl = '', maxTelegrams = 20000, refreshInterval = 2, resolveNames = true) {
    this.baseUrl = baseUrl;
    this.maxTelegrams = maxTelegrams;
    this.refreshInterval = refreshInterval;
    this.resolveNames = resolveNames;
  }

  addTelegrams(telegrams) {
    telegrams.forEach(t => t.flags = t.flags.split(' ').sort());

    // Add telegrams
    this.data.telegrams.splice(0, 0, ...telegrams);

    // Cap collection
    if (this.data.telegrams.length > this.maxTelegrams) {
      this.data.telegrams.splice(this.maxTelegrams, this.data.telegrams.length - this.maxTelegrams);
    }

    // Generate unique devices list
    let devices = new Set();
    this.data.telegrams.forEach(({ fromName, toName }) => {
      if (fromName && !devices.has(fromName)) devices.add(fromName);
      if (toName && !devices.has(toName)) devices.add(toName);
    });
    devices = [...devices].sort();
    this.data.devices.splice(0, this.data.devices.length, ...devices);
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
      setTimeout(() => this.autorefresh(), refreshInterval)
    }
    catch (err) {
      const msg = `API Error getLogByLogNumber: ${ err.message }`;
      if (!this.data.errors.includes(msg)) this.data.errors.unshift(msg);
      this.data.errorCnt++;
      if (this.data.errorCnt < 5) {
        setTimeout(() => this.autorefresh(), this.refreshInterval * 1000);
      } else {
        this.data.errors.unshift('Too many errors, telegram fetching stopped. Reload App to retry.')
      }
      console.error(err);
    }
  }

  async fetchLog(offset = 0) {
    const res = await this._fetch(`${ this.baseUrl }/getLogByLogNumber?lognum=${ offset }`);
    let json = await res.json();
    json = json.sort((a,b) => b.lognumber - a.lognumber);
    if (this.resolveNames) {
      json.forEach(t => this.addNameFromDevlist(t));
    }
    return json;
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
      telegram.fromIsIp = fromResolved.isIp;
    }
    const toResolved = this.resolveNameFromDevList(telegram.to);
    telegram.toNameResolved = toResolved !== null;
    if (toResolved) {
      telegram.toName = toResolved.name;
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


  async fetchDevList() {
    try {
      const blob = await (await this._fetch(`${ this.baseUrl }/getAskSinAnalyzerDevListJSON`, { cache: 'reload' })).blob();
      const filereader = new FileReader();
      const readed = new Promise(resolve => filereader.addEventListener('loadend', () => resolve(filereader.result)));
      filereader.readAsText(blob, 'iso-8859-1');
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
    document.location.href = `${ this.baseUrl }/httpupdate?url=https://raw.githubusercontent.com/jp112sdl/AskSinAnalyzer/master/ota/${ file }`;
  }
}
