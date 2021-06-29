<template>
  <div>
    <div class="text-red text-bold" v-if="!$root.espConfig">
      <q-icon name="warning"/>
      Die Konfiguration konnte nicht geladen werden.
    </div>
    <table class="esp-settings-table" v-else>
      <tr>
        <th>Device-List:</th>
        <td>{{ new Date($root.data.devlistCreated * 1000).toLocaleString()  }}</td>
      </tr>
      <tr>
        <th>Booted:</th>
        <td>{{ new Date($root.espConfig.boottime * 1000).toLocaleString() }}</td>
      </tr>
      <tr>
        <th>{{ $root.espConfig.staticipconfig ? 'Statische' : 'DHCP' }} IP:</th>
        <td>{{ $root.espConfig.ip }}</td>
      </tr>
      <tr>
        <th>Hostname:</th>
        <td>{{ $root.espConfig.hostname }}</td>
      </tr>
      <tr>
        <th>Netmask:</th>
        <td>{{ $root.espConfig.netmask }}</td>
      </tr>
      <tr>
        <th>Gateway:</th>
        <td>{{ $root.espConfig.gw }}</td>
      </tr>
      <tr>
        <th>NTP-Server:</th>
        <td>{{ $root.espConfig.ntp }}</td>
      </tr>
      <tr>
        <th>CCU-IP:</th>
        <td>{{ $root.espConfig.ccuip }}</td>
      </tr>
      <tr>
        <th>SD-Card:</th>
        <td>
          <div v-if="espConfig.sdcardavailable">
            {{ $root.espConfig.sdcardusedspacemb }} MB belegt / {{ $root.espConfig.sdcardtotalspacemb }} MB gesamt
          </div>
          <div v-else class="text-italic">Keine SD-Card</div>
        </td>
      </tr>
      <tr>
        <th>SPIFFS:</th>
        <td>{{ $root.espConfig.spiffsusedkb }} kB belegt / {{ $root.espConfig.spiffssizekb }} kB gesamt</td>
      </tr>
    </table>
<!--    <div class="q-mt-md">
      <q-btn icon="refresh" no-caps unelevated dense color="primary" @click="reloadConfig">
        &nbsp;Konfiguration neu laden&nbsp;
      </q-btn>
    </div>-->
  </div>
</template>

<script>
  import { QIcon, QBtn } from 'quasar';

  export default {
    name: "EspConfigInfo",
    components: { QIcon, QBtn },
    methods: {
      async reloadConfig() {
        await this.$espService.fetchConfig();
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
