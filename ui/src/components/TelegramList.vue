<template>
  <div>
    <div class="q-pb-md flex justify-between items-center">
      <div>
        {{ filtered.length }} von {{ value.length }} Telegramme
      </div>
    </div>
    <q-markup-table>
      <thead>
      <tr>
        <td class="text-left">Zeit</td>
        <td class="text-left">
          <q-btn label="Von" unelevated no-caps :color="filter.fromName.length ? 'secondary' : 'grey-7'" title="Von-Device Filter">
            <q-menu transition-show="jump-down" transition-hide="jump-up">
              <div class="q-pa-sm">
                <select-filter v-model="filter.fromName" :options="$root.data.devices" autofocus label="Devices"/>
              </div>
            </q-menu>
          </q-btn>
        </td>
        <td class="text-left">
          <q-btn label="An" unelevated no-caps :color="filter.toName.length ? 'secondary' : 'grey-7'" title="An-Device Filter">
            <q-menu transition-show="jump-down" transition-hide="jump-up">
              <div class="q-pa-sm">
                <select-filter v-model="filter.toName" :options="$root.data.devices" autofocus label="Devices"/>
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
          <q-btn label="Typ" unelevated no-caps :color="filter.types.length ? 'secondary' : 'grey-7'" title="RSSI Filter">
            <q-menu transition-show="jump-down" transition-hide="jump-up">
              <q-list dense>
                <q-item tag="label" v-for="v in types" :key="v">
                  <q-checkbox v-model="filter.types" :val="v" :label="v"/>
                </q-item>
              </q-list>
            </q-menu>
          </q-btn>
        </td>
        <td class="text-left">
          <q-btn label="Flags" flat no-caps></q-btn>
        </td>
      </tr>
      </thead>
      <tbody>
      <tr v-for="(v, i) in paginated" :key="i">
        <td class="text-left">{{ v.tstamp | date }}</td>
        <td class="text-left" :style="{ color: getDeviceColor(v, 'from') }">{{ v.fromName || v.from }}</td>
        <td class="text-left" :style="{ color: getDeviceColor(v, 'to') }">{{ v.toName || v.to }}</td>
        <td class="text-right">
          <rssi-value :value="v.rssi"/>
        </td>
        <td class="text-right">{{ v.len }}</td>
        <td class="text-right">{{ v.cnt }}</td>
        <td class="text-left">{{ v.typ }}</td>
        <td class="text-left">
          <flag-chip v-for="flag in v.flags" :key="flag" :value="flag"/>
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
  import { QBtn, QBtnGroup, QCheckbox, QItem, QList, QMarkupTable, QMenu, QPage, QPagination, QSelect } from 'quasar';
  import RssiValue from './RssiValue';
  import FlagChip from './FlagChip';
  import SelectFilter from './filters/SelectFilter';
  import RssiFilter from './filters/RssiFilter';

  export default {
    name: 'TelegramList',
    components: {
      QPage, QMarkupTable, QPagination, QBtn, QBtnGroup, QMenu, QSelect, QList, QItem, QCheckbox,
      RssiValue, FlagChip, SelectFilter, RssiFilter,
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
        filter: {
          start: null,
          stop: null,
          fromName: [],
          toName: [],
          rssi: [],
          types: []
        }
      }
    },

    computed: {
      filtered() {
        const { start, stop } = this.$root.timefilter;
        let result = this.value;
        if (start) result = result.filter(v => v.tstamp >= start);
        if (stop) result = result.filter(v => v.tstamp <= stop);
        if (this.filter.fromName.length) result = result.filter(v => this.filter.fromName.includes(v.fromName));
        if (this.filter.toName.length) result = result.filter(v => this.filter.toName.includes(v.toName));
        if (this.filter.rssi.length) {
          const ok = this.filter.rssi.includes('ok');
          const warn = this.filter.rssi.includes('warn');
          const crit = this.filter.rssi.includes('crit');
          result = result.filter(v => {
            if (ok && v.rssi >= -89) return true;
            if (warn && v.rssi <= -90 && v.rssi >= -109) return true;
            return crit && v.rssi <= -110;
          });
        }
        if (this.filter.types.length) result = result.filter(v => this.filter.types.includes(v.typ));
        return result;
      },
      paginated() {
        return this.filtered.slice((this.currPage - 1) * this.perPage, this.currPage * this.perPage - 1);
      },
      pages() {
        return Math.ceil(this.filtered.length / this.perPage);
      },
      types() {
        return [...new Set(this.value.map(v => v.typ))].sort()
      }
    },

    methods: {
      getDeviceColor(item, what) {
        if (!item[what + 'NameResolved']) return 'red';
        if (item[what + 'IsIp']) return '#027BE3';
        return 'black';
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
