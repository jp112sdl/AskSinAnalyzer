<template>
  <q-list dense >
    <q-item tag="label">
      <q-checkbox v-model="internalValue" val="ok" label="OK" color="green"/>
    </q-item>
    <q-item tag="label">
      <q-checkbox v-model="internalValue" val="warn" label="Warning" color="orange"/>
    </q-item>
    <q-item tag="label">
      <q-checkbox v-model="internalValue" val="crit" label="Critical" color="red"/>
    </q-item>
  </q-list>
</template>

<script>
  import { QList, QItem, QCheckbox } from 'quasar';

  export default {
    name: "RssiFilter",
    components: { QCheckbox, QList, QItem },

    props: {
      value: {
        type: Array,
      }
    },

    computed: {
      internalValue: {
        get() {
          return this.value || [];
        },
        set(v) {
          this.$emit('input', v);
        }
      }
    },
  }
</script>

<style lang="stylus" scoped>

</style>