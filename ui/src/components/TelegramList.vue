<template>
  <div>
    <div class="q-pb-md flex justify-between items-center">
      <div>
        {{ this.filtered.length }} von {{ this.value.length }} Telegramme
      </div>
      <div>
        <q-btn-group>
          <q-btn v-if="stopAtLognumber" color="secondary" icon="refresh" @click="refresh" title="Zeige aktuelle Telegramme"/>
          <q-btn
            :color="stopAtLognumber === null ? 'secondary' : 'amber'"
            :icon="stopAtLognumber === null ? 'autorenew' : 'play_arrow'"
            :class="{ 'q-btn__rotate-icon': stopAtLognumber === null }"
            no-caps
            :label="stopAtLognumber === null ? 'updating' : 'update'"
            @click="toggleRefresh"
            title="Auto-Update"
          />
        </q-btn-group>
      </div>
    </div>
    <q-markup-table>
      <thead>
      <tr>
        <td class="text-left">
          <q-btn label="Zeit" unelevated no-caps :color="filter.start || filter.stop ? 'secondary' : 'grey-7'" title="Zeit Filter">
            <q-menu transition-show="jump-down" transition-hide="jump-up">
              <div class="q-pa-sm">
                <time-filter v-model="filter.start" label="Von" autofocus class="q-mb-sm"/>
                <time-filter v-model="filter.stop" label="Bis"/>
              </div>
            </q-menu>
          </q-btn>
        </td>
        <td class="text-left">
          <q-btn label="Von" unelevated no-caps :color="filter.from.length ? 'secondary' : 'grey-7'" title="Von-Device Filter">
            <q-menu transition-show="jump-down" transition-hide="jump-up">
              <div class="q-pa-sm">
                <select-filter v-model="filter.from" :options="$root.devices" autofocus label="Devices"/>
              </div>
            </q-menu>
          </q-btn>
        </td>
        <td class="text-left">
          <q-btn label="An" unelevated no-caps :color="filter.to.length ? 'secondary' : 'grey-7'" title="An-Device Filter">
            <q-menu transition-show="jump-down" transition-hide="jump-up">
              <div class="q-pa-sm">
                <select-filter v-model="filter.to" :options="$root.devices" autofocus label="Devices"/>
              </div>
            </q-menu>
          </q-btn>
        </td>
        <td class="text-right">
          <q-btn label="RSSI" unelevated no-caps :color="filter.rssi.length ? 'secondary' : 'grey-7'" title="RSSI Filter">
            <q-menu transition-show="jump-down" transition-hide="jump-up">
              <rssi-filter v-model="filter.rssi"/>
            </q-menu>
          </q-btn>
        </td>
        <td class="text-right">Len</td>
        <td class="text-right">Cnt</td>
        <td class="text-left">
          <q-btn label="Type" flat no-caps></q-btn>
        </td>
        <td class="text-left">
          <q-btn label="Flags" flat no-caps></q-btn>
        </td>
      </tr>
      </thead>
      <tbody>
      <tr v-for="(v, i) in paginated" :key="i">
        <td class="text-left">{{ v.tstamp | date }}</td>
        <td class="text-left" :style="{ color: getDeviceColor(v.from) }">{{ v.from }}</td>
        <td class="text-left" :style="{ color: getDeviceColor(v.to) }">{{ v.to }}</td>
        <td class="text-right">
          <rssi-value :value="v.rssi"/>
        </td>
        <td class="text-right">{{ v.len }}</td>
        <td class="text-right">{{ v.cnt }}</td>
        <td class="text-left">{{ v.typ }}</td>
        <td class="text-left">
          <flag-chip v-for="flag in v.flags" :key="flag" :value="flag" />
        </td>
      </tr>
      </tbody>
    </q-markup-table>

    <div class="q-mt-md flex">
      <q-pagination
        style="display: inline-flex; margin-right: 2rem"
        v-model="currPage"
        :max="pages"
        :input="true"
      />
      <label>
        Telegrame pro Seite:
      <q-select
        v-model="perPage"
        filled
        dense
        :options="[10, 25,50,100,500]"
        style="display: inline-flex; margin-left: 0.3rem"
      />
      </label>
    </div>

  </div>
</template>

<style>
</style>

<script>
  import { QBtn, QBtnGroup, QMarkupTable, QMenu, QPage, QPagination, QSelect } from 'quasar';
  import RssiValue from './RssiValue';
  import FlagChip from './FlagChip';
  import TimeFilter from './filters/TimeFilter';
  import SelectFilter from './filters/SelectFilter';
  import RssiFilter from './filters/RssiFilter';

  export default {
    name: 'TelegramList',
    components: {
      QPage, QMarkupTable, QPagination, QBtn, QBtnGroup, QMenu, QSelect,
      RssiValue, FlagChip, TimeFilter, SelectFilter, RssiFilter,
    },

    props: {
      value: {
        type: Array,
        required: true,
        default: []
      }
    },

    data() {
      return {
        currPage: 1,
        perPage: 25,
        stopAtLognumber: null,
        filter: {
          start: null,
          stop: null,
          from: [],
          to: [],
          rssi: [],
        }
      }
    },

    computed: {
      filtered() {
        let result = this.value;
        if (this.stopAtLognumber) result = result.filter(v => v.lognumber <= this.stopAtLognumber);
        if (this.filter.start) result = result.filter(v => v.tstamp >= this.filter.start);
        if (this.filter.stop) result = result.filter(v => v.tstamp <= this.filter.stop);
        if (this.filter.from.length) result = result.filter(v => this.filter.from.includes(v.from));
        if (this.filter.to.length) result = result.filter(v => this.filter.to.includes(v.to));
        if (this.filter.rssi.length) {
          const ok = this.filter.rssi.includes('ok');
          const warn = this.filter.rssi.includes('warn');
          const crit = this.filter.rssi.includes('crit');
          result = result.filter(v => {
            if(ok && v.rssi >= -89) return true;
            if(warn && v.rssi <= -90 && v.rssi >= -109) return true;
            return crit && v.rssi <= -110;
          });
        }
        return result;
      },
      paginated() {
        return this.filtered.slice((this.currPage - 1) * this.perPage, this.currPage * this.perPage - 1);
      },
      pages() {
        return Math.ceil(this.filtered.length / this.perPage);
      }
    },

    watch: {
      currPage(v) {
        if (v > 1) this.refresh();
      }
    },

    methods: {
      toggleRefresh() {
        if (this.stopAtLognumber) {
          this.stopAtLognumber = null;
          this.currPage = 1;
        } else {
          this.refresh();
        }
      },
      refresh() {
        this.stopAtLognumber = this.value[0].lognumber;
      },
      getDeviceColor(v) {
        return (v.length === 6 && v !== '-ALLE-') ? 'red' : 'black';
      },
    }
  }
</script>

<style lang="stylus" scoped>
  thead
    .q-btn
      min-height 0
      line-height 1.3rem

  tbody td:nth-child(2)
    font-family monospace

  tbody td:nth-child(3)
    font-family monospace
</style>
