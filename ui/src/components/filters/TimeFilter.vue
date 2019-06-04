<template>
  <q-input
    filled
    fill-mask
    clearable
    mask="fulltime"
    dense
    v-model="internalValue"
    @blur="handleBlur"
    @focus="handleFocus"
    :label="label"
    :autofocus="autofocus"
  />
</template>


<script>
  import { QInput } from 'quasar';

  function pad(v) {
    return ('00' + v.toString()).slice(-2);
  }

  export default {
    name: "TimeFilter",
    components: { QInput, },

    props: {
      value: {
        type: Number
      },
      label: {
        type: String
      },
      autofocus: {
        type: Boolean
      }
    },

    data() {
      return {
        internalValue: ''
      }
    },

    watch: {
      // Transform tstamp to hh:mm:ss
      value: {
        immediate: true,
        handler(tstamp) {
          if (!tstamp) return this.internalValue = '';
          const d = new Date(tstamp * 1000);
          if (isNaN(d)) return this.internalValue = '';
          this.internalValue = `${ pad(d.getHours()) }:${ pad(d.getMinutes()) }:${ pad(d.getSeconds()) }`
        },
      },

      // Transform hh:mm:ss to tstamp end emit input
      internalValue(strTime) {
        if(!strTime || strTime === this.value) return;
        if (!strTime.match(/^\d\d:\d\d:\d\d$/)) {
          this.$emit('input', null);
          return;
        }
        const pieces = strTime.split(':').map(v => parseInt(v, 10));
        const d = new Date();
        d.setHours(pieces[0], pieces[1], pieces[2], 0);
        this.$emit('input', Math.floor(d.getTime() / 1000));
      }
    },

    methods: {
      // Clear input fld if it's invalid
      handleBlur() {
        if (!this.internalValue || !this.internalValue.match(/^\d\d:\d\d:\d\d$/)) {
          this.internalValue = null;
        }
      },
      handleFocus() {
        console.log('focus', this.internalValue);

        if (!this.internalValue || this.internalValue === '__:__:__') this.$el.querySelector('input').setSelectionRange(0, 1);
      }
    },

  };
</script>
