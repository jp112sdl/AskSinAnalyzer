<template>
  <div style="max-width: 350px">
    <q-list bordered separator>
      <q-item clickable v-ripple @click="reloadDevlist">
        <q-item-section avatar>
          <q-avatar rounded color="primary" text-color="white" icon="autorenew"/>
        </q-item-section>
        <q-item-section>
          <q-item-label class="text-bold">Reload DevList</q-item-label>
          <q-item-label caption>Lädt die Device-Liste erneut von der CCU.</q-item-label>
        </q-item-section>
      </q-item>
      <q-item clickable v-ripple @click="reboot">
        <q-item-section avatar>
          <q-avatar rounded color="primary" text-color="white" icon="settings_backup_restore"/>
        </q-item-section>
        <q-item-section>
          <q-item-label class="text-bold">Reboot</q-item-label>
          <q-item-label caption>Startet den ESP neu.</q-item-label>
        </q-item-section>
      </q-item>
      <q-item clickable v-ripple @click="rebootInConfigMode">
        <q-item-section avatar>
          <q-avatar rounded color="primary" text-color="white" icon="settings_applications"/>
        </q-item-section>
        <q-item-section>
          <q-item-label class="text-bold">Config-Modus</q-item-label>
          <q-item-label caption>Startet den ESP im Config-Modus neu.</q-item-label>
        </q-item-section>
      </q-item>
      <q-item clickable v-ripple @click="downloadcsv" v-if="sdcardavailable">
        <q-item-section avatar>
          <q-avatar rounded color="primary" text-color="white" icon="save_alt"/>
        </q-item-section>
        <q-item-section>
          <q-item-label class="text-bold">CSV Download</q-item-label>
          <q-item-label caption>CSV mit empfangenen Telegrammen herunterladen.</q-item-label>
        </q-item-section>
      </q-item>
      <q-item clickable v-ripple @click="deletecsv" v-if="sdcardavailable">
        <q-item-section avatar>
          <q-avatar rounded color="primary" text-color="white" icon="delete_sweep"/>
        </q-item-section>
        <q-item-section>
          <q-item-label class="text-bold">CSV Löschen</q-item-label>
          <q-item-label caption>Löscht die gespeicherten CSV Daten.</q-item-label>
        </q-item-section>
      </q-item>
      <q-item clickable v-ripple @click="csvUploadModal = true">
        <q-item-section avatar>
          <q-avatar rounded color="primary" text-color="white" icon="publish"/>
        </q-item-section>
        <q-item-section>
          <q-item-label class="text-bold">CSV Import</q-item-label>
          <q-item-label caption>Visualisiert CSV Daten in der WebUI.</q-item-label>
        </q-item-section>
      </q-item>
    </q-list>
    <q-dialog v-model="csvUploadModal">
      <csv-import @imported="csvUploadModal = false"/>
    </q-dialog>
  </div>
</template>

<script>
  import {
    QAvatar,
    QItem,
    QItemLabel,
    QItemSection,
    QList,
    Loading,
    QDialog,
  } from 'quasar';
  import CsvImport from './CsvImport';

  const wait = time => new Promise(res => setTimeout(res, time));


  export default {
    name: "EspCommands",
    components: { QAvatar, QList, QItem, QItemSection, QItemLabel, QDialog, CsvImport },

    data() {
      return {
        csvUploadModal: false
      }
    },

    computed: {
      sdcardavailable() {
        return this.$root.espConfig && this.$root.espConfig.sdcardavailable;
      }
    },

    methods: {
      async reloadDevlist() {
        Loading.show({
          message: '<b>Die Device-List wird neu geladen...</b>'
        });
        await this.$espService.fetchDevList();
        Loading.hide();
      },

      async reboot() {
        Loading.show({
          message: '<b>Der ESP wird neu gestartet...</b>'
        });
        await this.$espService.postCommand('reboot');
        await wait(5*1000);
        document.location.reload();
      },

      async rebootInConfigMode(){
        Loading.show({
          message: 'Der ESP wird im Config Modus gestartet.<br/>Verbinden Sie sich nun mit dem AskSinAnalyzer AccessPoint.'
        });
        await this.$espService.postCommand('rebootInConfigMode');
        await wait(5 * 1000);
        let cnt = 0;
        while (await this.$espService._fetch('http://192.168.4.1') === false && cnt < 30) {
          cnt++;
          await wait(1000);
        }
        document.location.href = 'http://192.168.4.1';
      },

      downloadcsv() {
        const a = document.createElement("a");
        a.style.display = "none";
        document.body.appendChild(a);
        a.href = this.$espService.baseUrl + '/downloadcsv';
        a.setAttribute("download", 'asksin-analyzer.csv');
        a.setAttribute("target", '_blank');
        a.click();
        document.body.removeChild(a);
      },

      async deletecsv() {
        Loading.show();
        await this.$espService.postCommand('deletecsv');
        await wait(2*1000);
        Loading.hide();
      },

      async ping() {
        let cnt = 0;
        while(await this.$espService.ping() === false && cnt < 20) {
          cnt++;
          await wait(500);
        }
        Loading.hide();
      }
    }
  }
</script>

<style scoped lang="stylus">

</style>
