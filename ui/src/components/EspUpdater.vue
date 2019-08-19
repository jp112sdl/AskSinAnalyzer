<template>
  <div>
    <div class="text-red text-bold q-mb-sm" v-if="!$root.espConfig">
      <q-icon name="warning"/>
      Die Konfiguration konnte nicht geladen werden.
    </div>
    <div v-else>
      <div class="text-blue text-bold q-mb-sm" v-if="$root.espConfig.updateAvailable">
        <q-icon name="info"/> Es ist ein Update verfügbar.
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
      <div class="q-mt-md" v-if="$root.espConfig.updateAvailable">
        <div class="q-my-md text-bold">
          <q-checkbox v-model="changelogGelesen"/>
          <a href="https://github.com/jp112sdl/AskSinAnalyzer/blob/master/CHANGELOG.md" target="_blank" ref="noreferrer noopener">CHANGELOG</a> gelesen
        </div>
        <q-btn label="Update durchführen" type="submit" color="primary" icon="system_update" @click="$espService.execUpdate()" :disabled="!changelogGelesen"/>
      </div>
    </div>
  </div>
</template>

<script>
  import { QIcon, QBtn, QCheckbox } from 'quasar';

  export default {
    name: "EspUpdater",
    components: { QIcon, QBtn, QCheckbox },
    data() {
      return {
        changelogGelesen: false
      }
    }
  }
</script>

<style scoped lang="stylus">
</style>
