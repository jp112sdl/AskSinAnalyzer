<template>
  <div>
    <div class="text-red text-bold q-mb-sm" v-if="!$root.espConfig">
      <q-icon name="warning"/>
      Die Konfiguration konnte nicht geladen werden.
    </div>
    <div v-else>
      <div class="text-blue text-bold q-mb-sm" v-if="$root.espConfig.updateAvailable">
        <q-icon name="info"/> Es ist ein Update verfügbar. 
        <p>Download: >
          <a href="https://github.com/jp112sdl/AskSinAnalyzer/releases" target="_blank" rel="noopener noreferrer">
            https://github.com/jp112sdl/AskSinAnalyzer/releases
          </a>
        </p>
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
        <q-btn label="Update durchführen" type="submit" color="primary" icon="system_update" @click="updateDialog = true"/>
      </div>
    </div>
    <q-dialog v-model="updateDialog">
      <q-card style="width: 700px; max-width: 100vw;">
        <q-card-section class="row items-center">
          <div class="text-h6">ESP Update</div>
          <q-space/>
          <q-btn icon="close" flat round dense v-close-popup/>
        </q-card-section>
        <q-card-section style="max-height: 60vh" class="scroll">
          <Changelog/>
        </q-card-section>
        <q-card-actions align="between">
          <q-btn label="abbrechen" color="warning" v-close-popup/>
          <q-btn label="Update durchführen" color="primary" @click="$espService.execUpdate()"/>
        </q-card-actions>
      </q-card>
    </q-dialog>
  </div>
</template>

<script>
  import { QIcon, QBtn, QCheckbox, QDialog, QCard, QCardSection, QCardActions, QSpace } from 'quasar';
  import Changelog from './Changelog';

  export default {
    name: "EspUpdater",
    components: { QIcon, QBtn, QCheckbox, QDialog, QCard, QCardSection, QCardActions, QSpace, Changelog },
    data() {
      return {
        updateDialog: false
      }
    }
  }
</script>

<style scoped lang="stylus">
</style>
