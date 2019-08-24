import Vue from 'vue'
import { Quasar, Ripple, ClosePopup, Loading } from 'quasar';
import lang from 'quasar/lang/de.js'
import './styles/quasar.styl'
import '@quasar/extras/material-icons/material-icons.css'

import App from './App.vue'
import './filter';
import router from './router'
import EspService from './EspService';

// Load Settings from localStorage
const defaultSettings = {
  maxTelegrams: 100000,
  refreshInterval: 1,
  espIp: '',
  resolveNames: true
};
const storedSettings = JSON.parse(localStorage.getItem('AskSinAnalyzer_Settings'));
const settings = { ...defaultSettings, ...storedSettings };

// Init EspService
const espService = new EspService(
  settings.espIp ? `http://${ settings.espIp }` : '',
  settings.maxTelegrams,
  settings.refreshInterval,
  settings.resolveNames
);
Vue.prototype.$espService = espService;


Vue.prototype.$debounce = function(fn, delay) {
  let timeoutID = null;
  return function() {
    clearTimeout(timeoutID);
    const args = arguments;
    const that = this;
    timeoutID = setTimeout(function() {
      fn.apply(that, args)
    }, delay)
  }
};

// Init Vue
Vue.use(Quasar, { lang, directives: { Ripple, ClosePopup } });
Vue.config.productionTip = false;

const vm = new Vue({
  router,
  data() {
    return {
      CDN: process.env.VUE_APP_CDN_URL || '',
      COMMIT: process.env.VUE_APP_COMMIT_HASH || 'dev',
      LATEST_COMMIT: null,
      data: espService.data,
      settings,
      espConfig: null,
      errors: {
        common: [],
        espService: espService.data.errors
      },
      timefilter: {
        start: null,
        stop: null
      }
    };
  },
  created() {
    Loading.show({
      message: '<b>AskSinAnalyzer lädt Daten vom ESP32</b>',
      customClass: 'main-loading'
    });
  },
  render: h => h(App)
}).$mount('#app');

// Fetch ESP Configuration
(async function() {
  try {
    vm.espConfig = await espService.fetchConfig();
    await espService.fetchDevList();
    // Periodically fetch new telegrams
    await espService.autorefresh();

    await espService.fetchVersion();
    if(vm.espConfig.updateAvailable) {
      vm.errors.common.push('ESP Update verfügbar.');
    }

    Loading.hide();

    try {
      const res = await fetch((vm.CDN || 'https://raw.githubusercontent.com/jp112sdl/AskSinAnalyzer/gh-pages/master') + '/commit-hash.txt', { cache: "no-store" });
      if (res.ok) {
        vm.LATEST_COMMIT = (await res.text()).trim();
        if(vm.LATEST_COMMIT !== vm.COMMIT) {
          vm.errors.common.push('WebUI Update verfügbar.');
        }
      } else {
        console.error(new Error(`${ res.status }: ${ res.statusText }`));
      }
    }
    catch (e) {
      e.message = `Network error while fetching latest commit from Github; ${ e.message }`;
      console.error(e);
    }

  } catch (e) {
    Loading.hide();
    console.error(e);
  }
})();


