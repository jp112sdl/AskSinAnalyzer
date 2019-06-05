<template>
  <div class="page">
    <h1>Einstellungen</h1>
    <p class="text-grey-8 text-italic">
      <q-icon name="info"/>
      Die Einstellungen werden im Browser gespeichert.
      Auf einem anderne Geräte müssen sie erneut gesetzt werden.
    </p>

    <q-form @submit="handleSubmit" @reset="handleReset" class="q-gutter-md q-mt-md">
      <q-input
        v-model="settings.maxTelegrams"
        outlined
        filled
        type="number"
        min="20"
        label="Max.-Telegramme"
        hint="Maximale Zahl der Telegramme die im Browser vorgehalten werden."
      />
      <q-input
        v-model="settings.refreshInterval"
        outlined
        filled
        type="number"
        min="1"
        label="Update Interval"
        hint="In diesen Zeitabständen werden neue Telegramme vom Analyzer aberufen."
        suffix="Sekunden"
      />
      <q-toggle label="Analyzer IP überschreiben" v-model="overwriteEspIp" />
      <q-input
        v-if="overwriteEspIp"
        v-model="settings.espIp"
        outlined
        filled
        type="text"
        label="Analyzer IP:Port"
        hint="IP Adresse und Port des Analyzers. Sollte normalerweise leer gelassen werden."
      />
      <div class="q-mt-lg">
        <q-btn label="speichern" type="submit" color="primary" icon="save"/>
        <q-btn label="Reset" type="reset" color="primary" flat class="q-ml-sm"/>
      </div>
    </q-form>

    <h2 class="q-mt-xl">ESP Konfiguration</h2>
    <div class="text-red text-bold" v-if="!$root.espConfig">
      <q-icon name="warning"/> Die Konfiguration konnte nicht geladen werden.
    </div>
    <table class="esp-settings-table" v-else>
      <tr>
        <th>IP:</th>
        <td>{{ $root.espConfig.staticip }}</td>
      </tr>
      <tr>
        <th>Netmask:</th>
        <td>{{ $root.espConfig.staticnetmask }}</td>
      </tr>
      <tr>
        <th>Gateway:</th>
        <td>{{ $root.espConfig.staticgateway }}</td>
      </tr>
      <tr>
        <th>CCU-IP:</th>
        <td>{{ $root.espConfig.ccuip }}</td>
      </tr>
      <tr>
        <th>Input Systemvariable:</th>
        <td>{{ $root.espConfig.svanalyzeinput }}</td>
      </tr>
      <tr>
        <th>Output Systemvariable:</th>
        <td>{{ $root.espConfig.svanalyzeoutput }}</td>
      </tr>
    </table>
  </div>
</template>

<script>
  import { QIcon, QForm, QInput, QBtn, QToggle } from 'quasar';

  export default {
    name: 'Settings',
    components: {
      QIcon, QForm, QInput, QBtn, QToggle,
    },

    data() {
      return {
        settings: { ...this.$root.settings },
        overwriteEspIp: this.$root.settings.espIp.length > 0,
      }
    },

    watch: {
      overwriteEspIp() {
        this.settings.espIp = '';
      }
    },

    methods: {
      handleSubmit() {
        localStorage.setItem('AskSinAnalyzer_Settings', JSON.stringify(this.settings));
        location.reload(true);
      },
      handleReset() {
        localStorage.removeItem('AskSinAnalyzer_Settings');
        location.reload(true);
      }
    }
  }
</script>

<style scoped lang="stylus">
  form
    max-width 450px

  .esp-settings-table
    th
      text-align left
      padding-right 1rem
    td
      font-family monospace
</style>
