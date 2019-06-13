export default class EspService {

  data = {
    telegrams: [],
    devices: [],
    espConfig: {},
    errors: [],
    errorCnt: 0,
  };
  maxTelegrams = 100;
  refreshInterval = 2;
  names = new Map();

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
    this.data.telegrams.forEach(({ from, to }) => {
      if (!devices.has(from)) devices.add(from);
      if (!devices.has(to)) devices.add(to);
    });
    devices = [...devices].sort();
    this.data.devices.splice(0, this.data.devices.length, ...devices);
  }

  async autorefresh() {
    try {
      const lastLognumber = this.data.telegrams[0] && this.data.telegrams[0].lognumber || 0;
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
    const json = await res.json();
    if(this.resolveNames) {
      const SNs = [... new Set(json.map(t => t.from).concat(json.map(t => t.to)))];
      await Promise.all(SNs.map(async sn => this.resolveName(sn)));
      json.forEach(t => {
        t.from = this.names.get(t.from) || t.from;
        t.to = this.names.get(t.to) || t.to;
      });
    }
    return json;
  }

  async fetchConfig() {
    try {
      const res = await this._fetch(`${ this.baseUrl }/getConfig`);
      this.data.espConfig = await res.json();
      return this.data.espConfig;
    }
    catch (err) {
      err.message = `API Error getConfig: ${ err.message }`;
      this.data.errors.unshift(err.message);
      this.data.errorCnt++;
      throw err;
    }
  }

  async postCommand(cmd) {
    return this._fetch(`${ this.baseUrl }/${ cmd }`, { method: 'post' });
  }

  async ping() {
    try {
      await this._fetch(this.baseUrl + '/index.html'); // TODO: better endpoint?
      return true;
    } catch(e) {
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

  async resolveName(serial) {
    if(['-ALLE-', '-ZENTRALE-'].includes(serial)) return null;
    if(this.names.has(serial)) return this.names.get(serial);
    if(serial.length !== 10) {
      this.names.set(serial, null);
      throw new Error(`Not resolving ${serial} due to wrong length`);
    }
    if(!this.data.espConfig.ccuip) throw new Error(`Not resolving ${serial} due to unknown CCU IP`);
    try {
      const blob = await (await this._fetch(`${ this.baseUrl }/getDeviceNameBySerial?Serial=${ serial }`)).blob();
      const filereader = new FileReader();
      const readed = new Promise(resolve => filereader.addEventListener('loadend', () => resolve(filereader.result)) );
      filereader.readAsText(blob, 'iso-8859-1');
      const name = await readed;
      this.names.set(serial, name);
      return name;
    } catch (e) {
      this.names.set(serial, null);
      console.error(e);
    }
    return null;
  }

}
