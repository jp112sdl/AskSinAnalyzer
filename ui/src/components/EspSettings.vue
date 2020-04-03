<template>
  <div>
    <q-form @submit="handleSubmit" @reset="handleReset" class="q-gutter-md q-mt-md">
      <div>
        <q-toggle
          label="Statische IP Konfiguration"
          v-model="cfg.staticipconfig"
          :true-value="1"
          :false-value="0"
        />
      </div>
      <q-input
        v-if="cfg.staticipconfig"
        v-model="cfg.ip"
        outlined
        filled
        type="string"
        label="IP-Adresse"
      />
      <q-input
        v-if="cfg.staticipconfig"
        v-model="cfg.netmask"
        outlined
        filled
        type="string"
        label="Netmask"
      />
      <q-input
        v-if="cfg.staticipconfig"
        v-model="cfg.gw"
        outlined
        filled
        type="string"
        label="Gateway"
      />
      <q-input
        v-model="cfg.ntp"
        outlined
        filled
        type="string"
        label="NTP-Server"
      />
      <q-input
        v-model="cfg.hostname"
        outlined
        filled
        type="string"
        label="Analyzer Hostname"
      />
      <q-toggle
        v-model="cfg.backend"
        label="Device-List Backend ist eine CCU"
        :true-value="0"
        :false-value="1"
      />
      <q-input
        v-if="!cfg.backend"
        v-model="cfg.ccuip"
        outlined
        filled
        type="string"
        label="CCU Adresse"
        hint="IP oder Hostname der CCU zum Laden der Geräteliste"
      />
      <q-input
        v-if="cfg.backend"
        v-model="cfg.backendurl"
        outlined
        filled
        type="string"
        label="Backend URL"
        hint="URL aus der die Geräteliste geladen werden soll"
      />
      <q-input
        v-model="cfg.rssi_hbw"
        outlined
        filled
        type="number"
        min="1"
        max="63"
        label="RSSI Histogram Bar Width"
        hint="Breite der RSSI Histogramm-Balken auf dem TFT"
      />
      <q-input
        v-model="cfg.rssi_alarmthreshold"
        outlined
        filled
        type="number"
        label="RSSI Alarm Threshold"
        hint="Schwellwert für RSSI Alarmmeldung in der CCU"
      />
      <q-input
        v-model="cfg.rssi_alarmcount"
        outlined
        filled
        type="number"
        label="RSSI Alarm Count"
        hint="Anzahl an stetigen Schwellwertüberschreitungen, bevor in der CCU der Alarm ausgelöst wird"
      />
      <div class="q-mt-lg">
        <q-btn label="speichern" type="submit" color="primary" icon="save" :loading="loading"/>
        <q-btn label="Reset" type="reset" color="primary" flat class="q-ml-sm"/>
      </div>
    </q-form>

  </div>
</template>

<script>
  import { QIcon, QForm, QInput, QBtn, QToggle } from 'quasar';

  const confProps = [
    'staticipconfig', 'ip', 'netmask', 'gw', 'ccuip', 'ntp', 'hostname',
    'backend', 'ccuip', 'backendurl', 'rssi_hbw', 'rssi_alarmthreshold', 'rssi_alarmcount'
  ];

  export default {
    name: 'EspSettings',
    components: {
      QIcon, QForm, QInput, QBtn, QToggle,
    },

    data() {
      const ret = {
        cfg: {},
        loading: false
      };
      confProps.forEach(prop => ret.cfg[prop] = null);
      return ret;
    },

    async mounted() {
      this.loading = true;
      await this.$espService.fetchConfig();
      this.updateObj(this.cfg);
      this.loading = false;
    },

    methods: {
      updateObj(obj) {
        confProps.forEach(prop => obj[prop] = this.$espService.data.espConfig[prop]);
      },
      async handleSubmit() {
        this.loading = true;
        if(!this.cfg.staticipconfig) {
          this.cfg.ip = '0.0.0.0';
        }
        const formData = new FormData();
        confProps.forEach(prop => formData.append(prop, this.cfg[prop]));
        await fetch(this.$espService.baseUrl + '/setConfig', {
          method: 'POST',
          body: formData
        });
        await this.$espService.fetchConfig();
        this.updateObj(this.cfg);
        this.loading = false;
      },
      handleReset() {
        this.updateObj(this.cfg);
      }
    }
  }
</script>

<style scoped lang="stylus">
  form
    max-width 450px
</style>
