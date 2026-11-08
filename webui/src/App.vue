<script setup lang="ts">
import { onMounted, ref } from 'vue'

import { call } from './bridge'

interface Hello {
  kind: string
  version: string
  host: string
  platform: string
}

const hello = ref<Hello | null>(null)
const problem = ref('')

onMounted(async () => {
  try {
    hello.value = await call<Hello>('ui.hello')
  } catch (error) {
    problem.value = error instanceof Error ? error.message : String(error)
  }
})
</script>

<template>
  <main class="shell">
    <header class="brand">
      <span class="logo" aria-hidden="true"></span>
      <h1>Plenio</h1>
    </header>
    <p v-if="hello" class="facts">
      {{ hello.kind === 'ara' ? 'Plenio ARA' : 'Plenio' }} {{ hello.version }} · {{ hello.host || 'unknown host' }}
    </p>
    <p v-else-if="problem" class="problem">{{ problem }}</p>
  </main>
</template>

<style scoped>
.shell {
  display: grid;
  place-content: center;
  gap: 12px;
  height: 100%;
  text-align: center;
}
.brand {
  display: flex;
  align-items: center;
  gap: 14px;
  justify-content: center;
}
.logo {
  width: 40px;
  height: 40px;
  border-radius: 12px;
  background: linear-gradient(135deg, var(--pink), var(--purple) 60%, var(--amber));
}
h1 {
  margin: 0;
  font-size: 34px;
  letter-spacing: 0.02em;
}
.facts {
  color: var(--text-dim);
}
.problem {
  color: var(--danger);
}
</style>
