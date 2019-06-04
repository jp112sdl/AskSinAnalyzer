import Vue from 'vue';

function pad(v) {
  return ('00' + v.toString()).slice(-2);
}

Vue.filter('date', v => {
  const d = new Date(v * 1000);
  return `${ pad(d.getHours()) }:${ pad(d.getMinutes()) }:${ pad(d.getSeconds()) }`;
});
