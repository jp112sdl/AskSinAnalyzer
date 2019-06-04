<template>
  <div class="page">
    <TelegramList :value="$root.telegrams"/>
  </div>
</template>

<script>
import TelegramList from '@/components/TelegramList.vue'

export default {
  name: 'home',
  components: {
    TelegramList
  }
}
</script>
