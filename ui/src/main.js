import Vue from 'vue'
import { Quasar, Ripple } from 'quasar';
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
  espIp: ''
};
const storedSettings = JSON.parse(localStorage.getItem('AskSinAnalyzer_Settings'));
const settings = { ...defaultSettings, ...storedSettings };

// Init EspService
const espService = new EspService(
  settings.espIp ? `http://${ settings.espIp }` : '',
  settings.maxTelegrams,
  settings.refreshInterval
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
Vue.use(Quasar, { lang, directives: { Ripple } });
Vue.config.productionTip = false;

const vm = new Vue({
  router,
  data() {
    return {
      data: espService.data,
      settings,
      espConfig: null,
      errors: {
        espService: espService.data.errors
      },
      timefilter: {
        start: null,
        stop: null
      }
    }
  },
  render: h => h(App)
}).$mount('#app');

// Fetch ESP Configuration
(async function() {
  try {
    vm.espConfig = await espService.fetchConfig();
  } catch (e) {
    console.error(e);
  }
})();

// Periodically fetch new telegrams
espService.autorefresh();
