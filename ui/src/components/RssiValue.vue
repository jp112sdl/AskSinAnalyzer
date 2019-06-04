<template>
  <span :style="{ backgroundColor: colors.bg, color: colors.fg }"> {{ value }} </span>
</template>

<script>
  export default {
    props: {
      value: {
        type: Number,
        required: true
      }
    },

    computed: {
      colors() {
        let p = 0;
        if(this.value >= -20) {
          p = 0;
        } else if(this.value >= -100) {
          p = 1 - (0.625 * this.value + 112.5) / 100;
        } else if (this.value < -100 && this.value >= -120) {
          p = 1 - (2.5 * this.value + 300) / 100;
        } else {
          p = 1;
        }
        const hue=((1-p)*120).toString(10);
        return {
          bg: `hsl(${ hue }, 75%, 50%)`,
          fg: hue < 37 ? 'white' : 'black'
        }
      }
    }
  }
</script>

<style lang="stylus" scoped>
  span
    padding 0.2rem 0.5rem
    border-radius 0.2rem

</style>
