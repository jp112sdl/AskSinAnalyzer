<template>
  <div>
    <div class="text-red text-bold q-mb-sm" v-if="!latestCommit">
      <q-icon name="warning"/>
      Die letzte Version konnte nicht ermittelt werden.
    </div>
    <div>
      <div class="text-blue text-bold q-mb-sm" v-if="updateAvailable">
        <q-icon name="info"/>
        Es ist ein Update verfügbar.
      </div>
      <div class="text-blue text-bold q-mb-sm" v-if="!updateAvailable && latestCommit">
        <q-icon name="check"/>
        Die WebUI ist auf dem neusten Stand.
      </div>
      <table>
        <tr>
          <th>Installierte Version:</th>
          <td>
            <code>{{ $root.COMMIT }}</code>
          </td>
        </tr>
        <tr v-if="latestCommit">
          <th>Verfügbare Version:</th>
          <td>
            <code>{{ latestCommit }}</code>
          </td>
        </tr>
      </table>
      <div class="q-mt-lg" v-if="updateAvailable">
        <q-btn label="Update durchführen" type="submit" color="primary" icon="system_update" @click="update"/>
      </div>
    </div>
  </div>
</template>

<script>
  import { QIcon, QBtn } from 'quasar';

  export default {
    name: "UiUpdater",
    components: { QIcon, QBtn },

    computed: {
      latestCommit() {
        return this.$root.LATEST_COMMIT;
      },
      updateAvailable() {
        return this.$root.LATEST_COMMIT && this.$root.LATEST_COMMIT !== this.$root.COMMIT;
      }
    },

    methods: {
      update() {
        location.reload(true);
      }
    }
  }
</script>

<style scoped lang="stylus">
</style>
