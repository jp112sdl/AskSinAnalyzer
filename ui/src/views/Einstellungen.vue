<template>
  <div class="page">
    <div class="row q-col-gutter-md">
      <div class="col-12 col-md-6">
        <q-card>
          <q-card-section>
            <h3>UI-Einstellungen</h3>
            <settings/>
          </q-card-section>
        </q-card>
      </div>
      <div class="col-12 col-md-6">
        <q-card>
          <q-card-section>
            <h3>ESP-Einstellungen</h3>
            <esp-settings/>
          </q-card-section>
        </q-card>
      </div>
    </div>
  </div>
</template>

<script>
  import { QCard, QCardSection } from 'quasar';

  import Settings from '@/components/Settings.vue';
  import EspSettings from '@/components/EspSettings.vue';

  export default {
    name: 'EinstellungenView',
    components: {
      QCard, QCardSection,
      Settings, EspSettings,
    },
  }
</script>

<style scoped lang="stylus">
</style>
