<template>
  <div>
    <div class="row q-col-gutter-md items-stretch">
      <div class="col-12 col-md-5 col-xl-4">
        <q-card>
          <pie-chart :data="sentByDevice"/>
        </q-card>
      </div>
      <div class="col-12 col-md-7 col-xl-4">
        <q-card style="height: 100%">
          <div class="q-pa-md">
            <h4>ESP Konfiguration</h4>
            <div class="row">
              <esp-config-info class="col-12 col-md-6 q-mb-md" :espConfig="$root.espConfig"/>
              <esp-commands class="col-12 col-md-6"/>
            </div>
          </div>
        </q-card>
      </div>
    </div>
  </div>
</template>

<script>
  import { QCard } from 'quasar';
  import PieChart from '@/components/PieChart.vue'
  import EspConfigInfo from '@/components/EspConfigInfo.vue';
  import EspCommands from '@/components/EspCommands.vue';


  export default {
    name: 'HomeView',
    components: {
      QCard,
      PieChart, EspConfigInfo, EspCommands,
    },

    computed: {
      sentByDevice() {
        const sentByDevice = {};
        const { start, stop } = this.$root.timefilter;
        this.$root.data.telegrams.forEach(t => {
          if (t.from === '-ALLE-') return;
          if (start && t.tstamp < start) return;
          if (stop && t.tstamp > stop) return;
          if (!sentByDevice[t.from]) {
            sentByDevice[t.from] = 1;
          } else {
            sentByDevice[t.from]++;
          }
        });
        return sentByDevice;
      }
      // devices() {
      //   return this.$root.data.devices.filter(v => v !== '-ALLE-');
      // }
    }
  }
</script>
