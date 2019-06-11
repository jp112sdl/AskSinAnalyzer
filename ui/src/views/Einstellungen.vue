<template>
  <div class="page">
    <h2>Einstellungen</h2>
    <q-card>
      <q-card-section>
        <settings/>
      </q-card-section>
    </q-card>
  </div>
</template>

<script>
  import { QCard, QCardSection } from 'quasar';

  import Settings from '@/components/Settings.vue';
  import EspConfigInfo from '@/components/EspConfigInfo.vue';
  import EspCommands from '@/components/EspCommands.vue';

  export default {
    name: 'EinstellungenView',
    components: {
      QCard, QCardSection,
      EspConfigInfo, EspCommands, Settings,
    },
  }
</script>

<style scoped lang="stylus">

</style>
