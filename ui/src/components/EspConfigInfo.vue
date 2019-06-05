<template>
  <div>
    <div class="text-red text-bold" v-if="!espConfig">
      <q-icon name="warning"/>
      Die Konfiguration konnte nicht geladen werden.
    </div>
    <table class="esp-settings-table" v-else>
      <tr>
        <th>IP:</th>
        <td>{{ espConfig.staticip }}</td>
      </tr>
      <tr>
        <th>Netmask:</th>
        <td>{{ espConfig.staticnetmask }}</td>
      </tr>
      <tr>
        <th>Gateway:</th>
        <td>{{ espConfig.staticgateway }}</td>
      </tr>
      <tr>
        <th>CCU-IP:</th>
        <td>{{ espConfig.ccuip }}</td>
      </tr>
      <tr>
        <th>Input Systemvariable:</th>
        <td>{{ espConfig.svanalyzeinput }}</td>
      </tr>
      <tr>
        <th>Output Systemvariable:</th>
        <td>{{ espConfig.svanalyzeoutput }}</td>
      </tr>
    </table>
  </div>
</template>

<script>
  import { QIcon } from 'quasar';

  export default {
    name: "EspConfigInfo",
    components: { QIcon },
    props: {
      espConfig: {
        type: Object
      }
    }
  }
</script>

<style scoped lang="stylus">
  .esp-settings-table
    th
      text-align left
      padding-right 1rem
    td
      font-family monospace
</style>