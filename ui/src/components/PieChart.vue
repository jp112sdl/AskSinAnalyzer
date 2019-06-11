<template>
  <div>
    <div ref="chart" style="height: 400px"></div>
  </div>
</template>

<script>
  import Highcharts from 'highcharts';

  export default {
    name: "PieChart",
    components: {},
    props: {
      data: {
        type: Object,
        required: true
      }
    },

    computed: {
      seriesData() {
        const d = [];
        Object.entries(this.data).forEach(([name, y]) => d.push({name, y}));
        return d.sort((a, b) => {
          if (a.name < b.name) return -1;
          if (a.name > b.name) return 1;
          return 0;
        });
      }
    },

    watch: {
      data: {
        immediate: false,
        handler() {
          this.updateData();
        }
      }
    },

    mounted() {
      this.hightchart = Highcharts.chart(this.$refs.chart, {
        chart: {
          plotBackgroundColor: null,
          plotBorderWidth: null,
          plotShadow: false,
          type: 'pie'
        },
        title: { text: 'Telegramme pro Device' },
        tooltip: {pointFormat: '{series.name}: <b>{point.y}</b>' },
        plotOptions: {
          pie: {
            allowPointSelect: true,
            cursor: 'pointer',
            dataLabels: {
              enabled: false,
            },
            showInLegend: true,
          }
        },
        legend: {
          align: 'right',
          verticalAlign: 'middle',
          layout: 'vertical',
          maxHeight: 330
        },
        series: [{
          name: 'Telegramme',
          colorByPoint: true,
          data: []
        }]
      });
      this.updateData();
    },

    data() {
      return {};
    },

    methods: {
      updateData() {
        this.hightchart.series[0].setData(this.seriesData, true, true);
      }
    }
  }
</script>

<style lang="stylus" scoped>

</style>
