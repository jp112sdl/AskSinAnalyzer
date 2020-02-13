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
        v-model="cfg.ccuip"
        outlined
        filled
        type="string"
        label="CCU IP"
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
      <div class="q-mt-lg">
        <q-btn label="speichern" type="submit" color="primary" icon="save" :loading="loading"/>
        <q-btn label="Reset" type="reset" color="primary" flat class="q-ml-sm"/>
      </div>
    </q-form>

  </div>
</template>

<script>
  import { QIcon, QForm, QInput, QBtn, QToggle } from 'quasar';

  const confProps = ['staticipconfig', 'ip', 'netmask', 'gw', 'ccuip', 'ntp', 'hostname'];

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
