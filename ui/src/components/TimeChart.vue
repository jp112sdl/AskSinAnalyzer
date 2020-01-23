<template>
  <div>
    <div ref="chart" style="height:300px"></div>
  </div>
</template>

<script>
  import Highcharts from 'highcharts/highstock';

  export default {
    name: "TimeChart",
    props: {
      data: {
        type: Array,
        required: true
      },
      rssiLog: {
        type: Array,
        required: true
      },
    },

    watch: {
      data: {
        immediate: false,
        handler() {
          this.updateData();
        }
      },
      rssiLog: {
        immediate: false,
        handler() {
          this.updateRssiLog();
        }
      },
    },

    mounted() {
      const $vm = this;
      const setTimeFilterDebounced = $vm.$debounce((min, max) => {
        $vm.$root.timefilter.start = Math.floor(min / 1000);
        $vm.$root.timefilter.stop = Math.ceil(max / 1000);
      }, 500);

      this.hightchart = Highcharts.stockChart(this.$refs.chart, {
        chart: {
          zoomType: 'x',
        },
        xAxis: {
          events: {
            afterSetExtremes(ev) {
              setTimeFilterDebounced(ev.min, ev.max);
            }
          }
        },
        time: { useUTC: false },
        rangeSelector: {
          inputEnabled: false,
          buttons: [{
            type: 'minute',
            count: 1,
            text: '1m'
          }, {
            type: 'minute',
            count: 10,
            text: '10m'
          }, {
            type: 'minute',
            count: 30,
            text: '30m'
          }, {
            type: 'hour',
            count: 1,
            text: '1h'
          }, {
            type: 'all',
            text: 'All'
          }]
        },
        title: { text: 'Telegramme' },
        exporting: { enabled: false },
        yAxis: [
          {
            opposite: false
          },
          {
            opposite: true,
            max: -40,
            min: -120,
          }
        ],
        series: [
          {
            name: 'Telegramme pro Sekunde',
            type: 'area',
            fillColor: {
              linearGradient: { x1: 0, y1: 0, x2: 0, y2: 1 },
              stops: [
                [0, Highcharts.getOptions().colors[0]],
                [1, Highcharts.Color(Highcharts.getOptions().colors[0]).setOpacity(0.2).get('rgba')]
              ]
            },
          },
          {
            name: 'RSSI Noise',
            type: 'line',
            color: '#ff5200',
            lineWidth: 1,
            yAxis: 1
          }
        ]
      });
      this.updateData();
    },

    data() {
      return {};
    },

    methods: {
      updateData() {
        if (!this.data.length) return;

        let m = new Map();
        this.data.forEach(t => {
          let cnt = m.get(t.tstamp) || 0;
          cnt++;
          m.set(t.tstamp, cnt);
        });
        let data = [];
        m.forEach((v, k) => data.push([k * 1000, v]));
        data = data.sort((a, b) => a[0] - b[0]);
        this.hightchart.series[0].setData(data, false);
        this.hightchart.redraw();
      },
      updateRssiLog() {
        if (!this.rssiLog.length) return;
        this.hightchart.series[1].setData(this.rssiLog, false);
        this.hightchart.redraw();
      }
    }
  }
</script>

<style lang="stylus" scoped>

</style>
