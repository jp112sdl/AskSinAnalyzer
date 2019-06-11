<template>
  <q-layout view="lHh Lpr lFf">
    <q-header elevated reveal>
      <q-toolbar>
        <q-toolbar-title class="q-mx-lg q-my-sm gt-xs" style="overflow: visible">
          <router-link to="/">
            AskSin Analyzer
          </router-link>
        </q-toolbar-title>

        <q-space class="lt-sm"/>

        <q-tabs>
          <q-route-tab icon="dashboard" to="/home" />
          <q-route-tab icon="list" to="/list" />
          <q-route-tab icon="settings" to="/settings" />
          <q-route-tab icon="info" to="/info" />
        </q-tabs>

        <q-space/>

        <q-btn icon="warning" color="red" flat dense v-if="errors.length">
          <q-menu auto-close transition-show="jump-down" transition-hide="jump-up">
            <q-list style="min-width: 250px">
              <q-item v-for="(err,i) in errors" :key="i">
                <q-item-section class="text-red">{{ err }}</q-item-section>
              </q-item>
            </q-list>
          </q-menu>
        </q-btn>

      </q-toolbar>

    </q-header>

    <q-page-container>
      <transition name="route" mode="out-in" :appear="true">
        <router-view/>
      </transition>
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
    QTabs, QRouteTab,
    QSpace,
    QMenu,
    QList, QItem, QItemSection,
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
      QTabs, QRouteTab,
      QSpace,
      QMenu,
      QList, QItem, QItemSection,
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
