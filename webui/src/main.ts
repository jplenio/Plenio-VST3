import { createApp } from 'vue'

import App from './App.vue'
import { insidePlugin, useMock } from './bridge'
import './styles/base.css'

async function start(): Promise<void> {
  // outside the plug-in (development, `npm run build:mock` for pictures and tests) a demo answers; the plug-in's own
  // build leaves the demo out
  if (!insidePlugin() && (import.meta.env.DEV || import.meta.env.MODE === 'mock')) {
    const { mockCall } = await import('./mock/mock')
    useMock(mockCall)
  }
  createApp(App).mount('#app')
}

void start()
