<template>
  <div>
    <div class="text-red text-bold" v-if="!espConfig">
      <q-icon name="warning"/>
      Die Konfiguration konnte nicht geladen werden.
    </div>
    <table class="esp-settings-table" v-else>
      <tr>
        <th>Booted</th>
        <td>{{ new Date(espConfig.boottime * 1000).toLocaleString() }}</td>
      </tr>
      <tr>
        <th>{{ espConfig.staticipconfig ? 'Statische' : 'DHCP' }} IP:</th>
        <td>{{ espConfig.ip }}</td>
      </tr>
      <tr>
        <th>Netmask:</th>
        <td>{{ espConfig.netmask }}</td>
      </tr>
      <tr>
        <th>Gateway:</th>
        <td>{{ espConfig.gateway }}</td>
      </tr>
      <tr>
        <th>CCU-IP:</th>
        <td>{{ espConfig.ccuip }}</td>
      </tr>
      <tr>
        <th>Input Sysvar:</th>
        <td>{{ espConfig.svanalyzeinput }}</td>
      </tr>
      <tr>
        <th>Output Sysvar:</th>
        <td>{{ espConfig.svanalyzeoutput }}</td>
      </tr>
      <tr>
        <th>SD-Card:</th>
        <td>
          <div v-if="espConfig.sdcardavailable">
            {{ espConfig.sdcardusedspacemb }} MB belegt / {{ espConfig.sdcardtotalspacemb }} MB gesamt
          </div>
          <div v-else class="text-italic">Keine SD-Card</div>
        </td>
      </tr>
      <tr>
        <th>SPIFFS:</th>
        <td>{{ espConfig.spiffsusedkb }} kB belegt / {{ espConfig.spiffssizekb }} kB gesamt</td>
      </tr>
    </table>
    <div class="q-mt-md">
      <q-btn icon="refresh" no-caps unelevated dense color="primary" @click="reloadConfig">
        &nbsp;Konfiguration neu laden&nbsp;
      </q-btn>
    </div>
  </div>
</template>

<script>
  import { QIcon, QBtn } from 'quasar';

  export default {
    name: "EspConfigInfo",
    components: { QIcon, QBtn },
    props: {
      espConfig: {
        type: Object
      }
    },
    methods: {
      async reloadConfig() {
        this.$root.espConfig = await this.$espService.fetchConfig();
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
