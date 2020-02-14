<template>
  <div class="page">
    <h1>AskSin Analyzer</h1>

    <q-card flat>
      <q-card-section>
        <p>Analyzer for radio telegrams in a HomeMatic environment</p>
        <p>
          <a href="https://github.com/jp112sdl/AskSinAnalyzer" target="_blank" rel="noopener noreferrer">
            <svg viewBox="0 0 1024 1024" fill="none" xmlns="http://www.w3.org/2000/svg">
              <path fill-rule="evenodd" clip-rule="evenodd" d="M8 0C3.58 0 0 3.58 0 8C0 11.54 2.29 14.53 5.47 15.59C5.87 15.66 6.02 15.42 6.02 15.21C6.02 15.02 6.01 14.39 6.01 13.72C4 14.09 3.48 13.23 3.32 12.78C3.23 12.55 2.84 11.84 2.5 11.65C2.22 11.5 1.82 11.13 2.49 11.12C3.12 11.11 3.57 11.7 3.72 11.94C4.44 13.15 5.59 12.81 6.05 12.6C6.12 12.08 6.33 11.73 6.56 11.53C4.78 11.33 2.92 10.64 2.92 7.58C2.92 6.71 3.23 5.99 3.74 5.43C3.66 5.23 3.38 4.41 3.82 3.31C3.82 3.31 4.49 3.1 6.02 4.13C6.66 3.95 7.34 3.86 8.02 3.86C8.7 3.86 9.38 3.95 10.02 4.13C11.55 3.09 12.22 3.31 12.22 3.31C12.66 4.41 12.38 5.23 12.3 5.43C12.81 5.99 13.12 6.7 13.12 7.58C13.12 10.65 11.25 11.33 9.47 11.53C9.76 11.78 10.01 12.26 10.01 13.01C10.01 14.08 10 14.94 10 15.21C10 15.42 10.15 15.67 10.55 15.59C13.71 14.53 16 11.53 16 8C16 3.58 12.42 0 8 0Z" transform="scale(64)" fill="#000"/>
            </svg>
            https://github.com/jp112sdl/AskSinAnalyzer
          </a>
        </p>
      </q-card-section>
    </q-card>

    <div class="row q-col-gutter-md q-mt-sm">
      <div class="col-12 col-md-6">
        <q-card>
          <q-card-section>
            <h3>UI-Version</h3>
            <ui-updater/>
          </q-card-section>
        </q-card>
      </div>
      <div class="col-12 col-md-6">
        <q-card>
          <q-card-section>
            <h3>ESP-Version</h3>
            <esp-updater/>
          </q-card-section>
        </q-card>
      </div>
    </div>

    <h2 class="q-mt-xl">FAQ</h2>

    <q-card flat>
      <q-card-section>
        <div class="q-mb-md">
          <div class="text-bold">Device-Serial ist rot</div>
          <div class="q-mb-md">Es wird die Adresse des Telegrams angezeigt da die Seriennummer nicht über die CCU aufgelöst werden konnte. Der Sender ist nicht an die CCU angelernt. Können gar keine Seriennummern ermittelt werden ist die CCU-IP sowie die Systemvariablen zu prüfen.
          </div>
          <div class="text-bold">Network Error! Verify Analyzer IP</div>
          <div class="q-mb-md">
            Es kann keine Verbindung zum ESP hergestellt werden.<br/>
            Dies kann mehrere Ursachen haben. Zuerst prüft man ob die Config direkt abgerufen werden:
            <code>http://&lt;EspIp&gt;/getConfig</code>. Funktioniert dies kann die Developer-Console des Browser
            zur weiteren Fehleranalyse herangezogen werden. <br/>
            Wird die WebUI über eine sichere Verbindung (http<strong>s</strong>) ausgeliefert,
            dann blockieren die meisten Browser eine unsichere Verbindung (http). Man muss dies für die
            WebUI zulassen, oft ist in der Adressbar ein Icon, was darauf hinweist.
          </div>
          <div class="text-bold">Flags</div>
          <div class="q-mb-md">
            <span class="flag-chip"><flag-chip value="WKUP"/></span>
            <span class="flag-desc">Receiver shall stay awake for short time.</span>
            <br/>
            <span class="flag-chip"><flag-chip value="WKMEUP"/></span>
            <span class="flag-desc">Sender indicates that it is still awake for short time.</span>
            <br/>
            <span class="flag-chip"><flag-chip value="BCAST"/></span>
            <span class="flag-desc">Telegram is a broadcast to all.</span>
            <br/>
            <span class="flag-chip"><flag-chip value="BURST"/></span>
            <span class="flag-desc">Telegram has been sent with a burst.</span>
            <br/>
            <span class="flag-chip"><flag-chip value="BIDI"/></span>
            <span class="flag-desc">Telegram must be acknowledged by the receiver for bidirectional communication.</span>
            <br/>
            <span class="flag-chip"><flag-chip value="RPTED"/></span>
            <span class="flag-desc">Telegram has already been repeated by a repeater.</span>
            <br/>
            <span class="flag-chip"><flag-chip value="RPTEN"/></span>
            <span class="flag-desc">Telegram is allowed to be repeated by a repeater.</span>
          </div>
        </div>
      </q-card-section>
    </q-card>


    <h2 class="q-mt-xl">Changelog</h2>
    <q-card flat>
      <q-card-section>
        <Changelog/>
      </q-card-section>
    </q-card>

    <h2 class="q-mt-xl">Tech</h2>
    <q-card flat>
      <q-card-section>
        <div class="tech-logos">
          <a href="http://vuejs.org" target="_blank" rel="noopener noreferrer">
            <img :src="$root.CDN + '/logos/Vue.js.svg'"> Vue.js
          </a>
          <a href="https://quasar.dev" target="_blank" rel="noopener noreferrer">
            <img :src="$root.CDN + '/logos/Quasar.svg'"> Quasar
          </a>
          <a href="https://www.highcharts.com" target="_blank" rel="noopener noreferrer">
            <img :src="$root.CDN + '/logos/Highcharts.png'"> Highcharts
          </a>
        </div>
      </q-card-section>
    </q-card>

    <h2 class="q-mt-xl">Lizenz</h2>
    <q-card flat>
      <q-card-section>
        <a href="https://creativecommons.org/licenses/by-nc-sa/3.0/de/" rel="noopener noreferrer" target="_blank">
          CC BY-NC-SA 3.0 DE
        </a>
        <br/>
        It is NOT free for commercial and governmental use!
      </q-card-section>
    </q-card>

    <h2 class="q-mt-xl">Authors</h2>
    <q-card flat>
      <q-card-section>
        <ul style="margin: 0;">
          <li>
            <a href="https://github.com/jp112sdl" target="_blank" rel="noopener noreferrer">
              Jérôme Pech
            </a>
          </li>
          <li>
            <a href="https://psi.cx" target="_blank" rel="noopener noreferrer">
              Christoph Wiechert
            </a>
          </li>
        </ul>
      </q-card-section>
    </q-card>
  </div>
</template>

<script>
  import { QCard, QCardSection } from 'quasar';
  import FlagChip from '../components/FlagChip';
  import Changelog from '../components/Changelog';
  import EspUpdater from '@/components/EspUpdater.vue';
  import UiUpdater from '@/components/UiUpdater.vue';

  export default {
    name: 'InfoView',
    components: { QCard, QCardSection, FlagChip, Changelog, EspUpdater, UiUpdater }
  }
</script>

<style lang="stylus" scoped>
  a
    > img, > svg
      vertical-align bottom
      width: 2rem
      height 2rem
      margin-right 0.3rem

  .tech-logos
    > a
      margin-right 1.5rem

  .flag-chip
    display inline-block
    width 70px
    margin-right 10px
</style>
