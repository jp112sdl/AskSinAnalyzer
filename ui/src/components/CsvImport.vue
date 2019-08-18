<template>
  <div>
    <q-card style="width: 340px">
      <q-card-section class="row items-center">
        <div class="text-h6">CSV Datei importieren</div>
        <q-space/>
        <q-btn icon="close" flat round dense v-close-popup/>
      </q-card-section>
      <q-card-section>
        <input type="file" ref="file" accept=".csv"/>
      </q-card-section>

      <q-card-actions class="bg-white" align="right">
        <q-btn size="small" color="primary" label="importieren" @click="importCsv"/>
      </q-card-actions>
    </q-card>
  </div>
</template>

<script>
  import {
    QCard,
    QCardSection,
    QCardActions,
    QSpace,
    QBtn,
  } from 'quasar';

  export default {
    name: "CsvImport",
    components: { QCard, QCardSection, QCardActions, QSpace, QBtn },

    methods: {
      importCsv() {
        const file = this.$refs.file && this.$refs.file.files[0];
        if(!file) return;
        const fileReader = new FileReader();
        fileReader.onload = fileLoadedEvent => {
          this.$espService.stopAutorefresh();
          this.$espService.data.telegrams = [];
          this.$espService.resolveNames = false;
          const strCsv = fileLoadedEvent.target.result;
          this.$espService.data.telegrams = strCsv
            .split(/\r?\n/)
            .map(line => {
              if(line.length < 5) return null;
              let [lognumber, datum, rssi, from, fromName, to, toName, len, cnt, typ, flags ] = line.split(';');
              const datumParts = datum.match(/^(\d\d)\.(\d\d)\.(\d\d\d\d) (\d\d):(\d\d):(\d\d)/);
              const tstamp = Math.round(new Date(datumParts[3], datumParts[2]-1, datumParts[1], datumParts[4], datumParts[5], datumParts[6]).getTime() / 1000);
              lognumber = +lognumber;
              rssi = +rssi;
              cnt = +cnt;
              len = +len;
              flags = flags.split(" ");
              return {
                lognumber,
                tstamp,
                rssi,
                from,
                fromName,
                to,
                toName,
                len,
                cnt,
                typ,
                flags
              };
            })
            .filter(obj => obj !== null)
            .reverse();
          this.$emit('imported');
        };
        fileReader.readAsText(file, "UTF-8");
      }
    }
  }
</script>

<style lang="stylus" scoped>

</style>
