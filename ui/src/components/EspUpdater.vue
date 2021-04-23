<template>
  <div>
    <div class="text-red text-bold q-mb-sm" v-if="!$root.espConfig">
      <q-icon name="warning"/>
      Die Konfiguration konnte nicht geladen werden.
    </div>
    <div v-else>
      <div class="text-blue text-bold q-mb-sm" v-if="$root.espConfig.updateAvailable">
        <q-icon name="info"/> Es ist ein Update verfügbar. 
        <p>Download</p>
        <p>
          <a href="https://github.com/jp112sdl/AskSinAnalyzer/releases" target="_blank" rel="noopener noreferrer">
            <svg viewBox="0 0 1024 1024" fill="none" xmlns="http://www.w3.org/2000/svg">
              <path fill-rule="evenodd" clip-rule="evenodd" d="M8 0C3.58 0 0 3.58 0 8C0 11.54 2.29 14.53 5.47 15.59C5.87 15.66 6.02 15.42 6.02 15.21C6.02 15.02 6.01 14.39 6.01 13.72C4 14.09 3.48 13.23 3.32 12.78C3.23 12.55 2.84 11.84 2.5 11.65C2.22 11.5 1.82 11.13 2.49 11.12C3.12 11.11 3.57 11.7 3.72 11.94C4.44 13.15 5.59 12.81 6.05 12.6C6.12 12.08 6.33 11.73 6.56 11.53C4.78 11.33 2.92 10.64 2.92 7.58C2.92 6.71 3.23 5.99 3.74 5.43C3.66 5.23 3.38 4.41 3.82 3.31C3.82 3.31 4.49 3.1 6.02 4.13C6.66 3.95 7.34 3.86 8.02 3.86C8.7 3.86 9.38 3.95 10.02 4.13C11.55 3.09 12.22 3.31 12.22 3.31C12.66 4.41 12.38 5.23 12.3 5.43C12.81 5.99 13.12 6.7 13.12 7.58C13.12 10.65 11.25 11.33 9.47 11.53C9.76 11.78 10.01 12.26 10.01 13.01C10.01 14.08 10 14.94 10 15.21C10 15.42 10.15 15.67 10.55 15.59C13.71 14.53 16 11.53 16 8C16 3.58 12.42 0 8 0Z" transform="scale(64)" fill="#000"/>
            </svg>
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
