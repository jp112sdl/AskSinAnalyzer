<template>
  <div>
    <telegram-list :value="$root.data.telegrams"/>
  </div>
</template>

<script>

  import TelegramList from '@/components/TelegramList.vue'

  export default {
  name: 'TelegramListView',
  components: {
    TelegramList,
  }
}
</script>
