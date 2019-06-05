export default class EspService {

  telegrams = [];
  devices = [];
  maxTelegrams = 100;
  errors = [];
  errorCnt = 0;
  refreshInterval = 2;

  constructor(baseUrl = '', maxTelegrams = 20000, refreshInterval = 2) {
    this.baseUrl = baseUrl;
    this.maxTelegrams = maxTelegrams;
    this.refreshInterval = refreshInterval;
  }

  async autorefresh() {
    try {
      const lastLognumber = this.telegrams[0] && this.telegrams[0].lognumber || 0;
      let telegrams = await this.fetchLog(lastLognumber);
      this.errors = [];
      this.errorCnt = 0;
      if (telegrams.length) this.addTelegrams(telegrams);
      setTimeout(() => this.autorefresh(), this.refreshInterval * 1000)
    }
    catch (err) {
      const msg = `API Error getLogByLogNumber: ${ err.message }`;
      if (!this.errors.includes(msg)) this.errors.unshift(msg);
      this.errorCnt++;
      if (this.errorCnt < 5) {
        setTimeout(() => this.autorefresh(), this.refreshInterval * 1000);
      } else {
        this.errors.unshift('Too many errors, telegram fetching stopped. Reload App to retry.')
      }
      console.error(err);
    }
  }

  async fetchLog(offset = 0) {
    const res = await this._fetch(`${ this.baseUrl }/getLogByLogNumber?lognum=${ offset }`);
    return await res.json();
  }

  async fetchConfig() {
    try {
      const res = await this._fetch(`${ this.baseUrl }/getConfig`);
      return await res.json();
    } catch(err) {
      err.message = `API Error getConfig: ${err.message}`;
      this.errors.unshift(err.message);
      this.errorCnt++;
      throw err;
    }
  }

  async _fetch(opts) {
    try {
      const res = await fetch(opts);
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

  addTelegrams(telegrams) {
    telegrams.forEach(t => t.flags = t.flags.split(' '));

    // Add telegrams
    this.telegrams.splice(0, 0, ...telegrams);

    // Cap collection
    if (this.telegrams.length > this.maxTelegrams) {
      this.telegrams.splice(this.maxTelegrams, this.telegrams.length - this.maxTelegrams);
    }

    // Generate unique devices list
    let devices = new Set();
    this.telegrams.forEach(({ from, to }) => {
      if (!devices.has(from)) devices.add(from);
      if (!devices.has(to)) devices.add(to);
    });
    devices = [...devices].sort();
    this.devices.splice(0, this.devices.length, ...devices);
  }

}
