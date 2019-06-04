<template>
  <q-select
    v-model="val"
    :options="opts"
    :label="label"
    use-input
    input-debounce="1"
    @filter="filterFn"
    multiple
    use-chips
    stack-label
    filled
    clearable
    :autofocus="autofocus"
    style="min-width: 200px"
  />
</template>


<script>
  import { QSelect } from 'quasar';

  export default {
    name: "SelectFilter",
    components: { QSelect, },

    props: {
      value: {
        type: [String, Array]
      },
      options: {
        type: Array
      },
      label: {
        type: String
      },
      autofocus: {
        type: Boolean
      },
    },

    computed: {
      val: {
        get() {
          return this.value
        },
        set(val) {
          if(val === null) val = [];
          this.$emit('input', val);
        }
      }
    },

    data() {
      return {
        opts: this.options
      }
    },

    methods: {
      filterFn(val, update) {
        if (val === '') {
          update(() => this.opts = this.options);
          return
        }

        update(() => {
          const needle = val.toLowerCase();
          this.opts = this.options.filter(v => v.toLowerCase().indexOf(needle) > -1)
        })
      }
    }

  };
</script>
