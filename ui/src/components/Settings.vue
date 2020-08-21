<template>
  <div>
    <p class="text-grey-8 text-italic">
      <q-icon name="info"/>
      Die Einstellungen werden im Browser gespeichert.
      Auf einem anderen Gerät müssen sie erneut gesetzt werden.
    </p>
    <q-form @submit="handleSubmit" @reset="handleReset" class="q-gutter-md q-mt-md">
      <div>
        <q-toggle
          label="CCU Namen auflösen"
          v-model="settings.resolveNames"
        />
        <div class="q-field__bottom">
          Auflösen und Anzeige der ReGa-Namen anstelle der Seriennummer.
        </div>
      </div>
      <div>
        <q-toggle
          label="Unbekannte Geräte ignorieren"
          v-model="settings.ignoreUnknownDevices"
        />
        <div class="q-field__bottom">
          Es werden alle Telegramme von Geräten ignoriert, die nicht durch die Device-List aufgelöst werden können.
        </div>
      </div>
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
        hint="In diesen Zeitabständen werden neue Telegramme vom Analyzer abgerufen."
        suffix="Sekunden"
      />
      <q-toggle label="Analyzer IP überschreiben" v-model="overwriteEspIp"/>
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
</style>
