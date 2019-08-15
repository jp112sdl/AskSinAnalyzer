<template>
  <div>
    <div class="text-red text-bold q-mb-sm" v-if="!$root.espConfig">
      <q-icon name="warning"/>
      Die Konfiguration konnte nicht geladen werden.
    </div>
    <div v-else>
      <div class="text-blue text-bold q-mb-sm" v-if="$root.espConfig.updateAvailable">
        <q-icon name="info"/>
        Es ist ein Update verfügbar.
      </div>
      <div class="text-blue text-bold q-mb-sm" v-else>
        <q-icon name="check"/>
        Die ESP Firmware ist auf dem neusten Stand.
      </div>
      <table>
        <tr>
          <th>Installierte Version:</th>
          <td>
            <code>{{ $root.espConfig.currentVersion }}</code>
          </td>
        </tr>
        <tr>
          <th>Verfügbare Version:</th>
          <td>
            <code>{{ $root.espConfig.latestVersion }}</code>
          </td>
        </tr>
      </table>
      <div class="q-mt-lg" v-if="$root.espConfig.updateAvailable">
        <q-btn label="Update durchführen" type="submit" color="primary" icon="system_update" @click="$espService.execUpdate()"/>
      </div>
    </div>
  </div>
</template>

<script>
  import { QIcon, QBtn } from 'quasar';

  export default {
    name: "EspUpdater",
    components: { QIcon, QBtn },
  }
</script>

<style scoped lang="stylus">
</style>
