<template>
  <div class="page">
    <div class="row q-col-gutter-md">
      <div class="col-12 col-md-6">
        <q-card>
          <q-card-section>
            <h2>Einstellungen</h2>
            <settings/>
          </q-card-section>
        </q-card>
      </div>
      <div class="col-12 col-md-6">
        <q-card>
          <q-card-section>
            <h2>ESP-Version</h2>
            <esp-updater/>
          </q-card-section>
        </q-card>
        <q-card class="q-mt-md">
          <q-card-section>
            <h2>UI-Version</h2>
            <ui-updater/>
          </q-card-section>
        </q-card>
      </div>
    </div>
  </div>
</template>

<script>
  import { QCard, QCardSection } from 'quasar';

  import Settings from '@/components/Settings.vue';
  import EspUpdater from '@/components/EspUpdater.vue';
  import UiUpdater from '@/components/UiUpdater.vue';

  export default {
    name: 'EinstellungenView',
    components: {
      QCard, QCardSection,
      Settings, EspUpdater, UiUpdater,
    },
  }
</script>

<style scoped lang="stylus">
</style>
