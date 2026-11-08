import { createApp } from 'vue'

import App from './App.vue'
import { insidePlugin, useMock } from './bridge'
import './styles/base.css'

async function start(): Promise<void> {
  if (!insidePlugin()) {
    const { mockCall } = await import('./mock/mock')
    useMock(mockCall)
  }
  createApp(App).mount('#app')
}

void start()
