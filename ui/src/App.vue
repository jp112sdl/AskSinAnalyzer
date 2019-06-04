<template>
  <q-layout view="lHh Lpr lFf">
    <q-header elevated reveal>
      <q-toolbar>
        <q-toolbar-title class="q-mx-lg q-my-sm">
          <router-link to="/">
            AskSin Analyzer
          </router-link>
        </q-toolbar-title>

        <!--
        <q-tabs v-model="tab">
          <q-tab name="devices" label="Geräte" />
          <q-tab name="telegrams" label="Telegramme" />
          <q-tab name="ka?" label="Fubar"/>
        </q-tabs>
        -->

        <q-space/>

        <q-btn icon="warning" color="red" flat dense v-if="errors.length">
          <q-menu auto-close>
            <q-list style="min-width: 250px">
              <q-item v-for="(err,i) in errors" :key="i">
                <q-item-section class="text-red">{{ err }}</q-item-section>
              </q-item>
            </q-list>
          </q-menu>
        </q-btn>

        <q-btn flat dense>
          <q-icon name="more_vert"/>
          <q-menu auto-close>
            <q-list style="min-width: 100px">
              <q-item clickable to="/settings">
                <q-item-section>Einstellungen</q-item-section>
              </q-item>
              <q-item clickable to="/info">
                <q-item-section>Info</q-item-section>
              </q-item>
            </q-list>
          </q-menu>
        </q-btn>
      </q-toolbar>

    </q-header>

    <q-page-container>
      <router-view />
    </q-page-container>
  </q-layout>
</template>

<script>
  import {
    QHeader,
    QLayout,
    QPageContainer,
    QToolbar,
    QToolbarTitle,
    QIcon,
    QBtn,
    QTabs, QTab,
    QSpace,
    QMenu,
    QList, QItem,
    QItemSection,
  } from 'quasar';

  export default {
    name: 'LayoutDefault',

    components: {
      QHeader,
      QLayout,
      QPageContainer,
      QToolbar,
      QToolbarTitle,
      QIcon,
      QBtn,
      QTabs, QTab,
      QSpace,
      QMenu,
      QList, QItem,
      QItemSection,
    },

    data() {
      return {
        tab: "images",
      }
    },

    computed: {
      errors() {
        return Object.values(this.$root.errors).flat();
      }
    }
  }
</script>

<style lang="stylus" scoped>
  .q-toolbar a
    line-height 1;
  .q-toolbar__title a
    text-decoration none
    color white
</style>
