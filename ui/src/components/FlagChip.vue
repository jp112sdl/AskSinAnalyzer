<template>
  <span :style="{ backgroundColor, color }">{{ value }}</span>
</template>

<script>
  export default {
    name: "FlagChip",

    props: {
      value: {
        type: String
      }
    },

    computed: {
      backgroundColor() {
        switch(this.value) {
          case "WKUP":
            return '#b0b4ff';
          case "WKMEUP":
            return '#8ac1ff';
          case "BCAST":
            return '#139325';
          case "BURST":
            return '#da0000';
          case "BIDI":
            return '#ffc971';
          case "RPTED":
            return '#cbcbcb';
          case "RPTEN":
            return '#CBCBCB';
        }
        return '#F2F2F2';
      },
      color() {
        const r = parseInt(this.backgroundColor.slice(1,3), 16);
        const g = parseInt(this.backgroundColor.slice(3,5), 16);
        const b = parseInt(this.backgroundColor.slice(5,7), 16);
        return Math.round(((r * 299) + (g * 587) + (b * 114)) / 1000) > 128 ? 'back' : 'white';
      }
    }
  }
</script>

<style scoped lang="stylus">
  span
    margin-right: 0.2rem
    margin-bottom: 0.1rem
    border-radius 4px
    padding 0.15rem 0.3rem
    font-size: 90%
</style>