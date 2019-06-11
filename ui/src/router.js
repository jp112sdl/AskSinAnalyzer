import Vue from 'vue'
import Router from 'vue-router'

import Home from './views/Home.vue'
import WithTimeChart from './views/WithTimeChart.vue'
import TelegramList from './views/TelegramList.vue'
import Einstellungen from './views/Einstellungen.vue'
import Info from './views/Info.vue'
import Page404 from './views/404.vue'

Vue.use(Router);

export default new Router({
  routes: [
    {
      path: '/',
      redirect: '/home'
    },
    {
      path: '/home',
      component: WithTimeChart,
      children: [
        {
          path: '',
          component: Home
        },
        {
          path: '/list',
          component: TelegramList
        },
      ]
    },
    {
      path: '/settings',
      component: Einstellungen
    },
    {
      path: '/info',
      component: Info
    },
    {
      path: '*',
      component: Page404
    }
  ]
})
