<template>
  <div class="page">
    <h2>ESP Konfiguration</h2>
    <div class="row">
      <div class="col">
        <esp-config-info :espConfig="$root.espConfig"/>
      </div>
      <div class="col">
        <esp-commands/>
      </div>
    </div>

    <h2 class="q-mt-lg">Einstellungen</h2>
    <settings />
  </div>
</template>

<script>
  import Settings from '@/components/Settings.vue';
  import EspConfigInfo from '@/components/EspConfigInfo.vue';
  import EspCommands from '@/components/EspCommands.vue';

  export default {
    name: 'Einstellungen',
    components: {
      EspConfigInfo, EspCommands, Settings,
    },
  }
</script>

<style scoped lang="stylus">

</style>
