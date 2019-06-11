<template>
  <div class="page">
    <q-card class="q-mb-lg">
      <time-chart :data="$root.data.telegrams"/>
    </q-card>
    <transition name="route" mode="out-in" :appear="true">
      <router-view/>
    </transition>
  </div>
</template>


<script>
  import { QCard } from 'quasar';
  import TimeChart from '@/components/TimeChart.vue'

  export default {
    name: "WithTimeChart",
    components: {
      QCard, TimeChart
    },
  };
</script>

