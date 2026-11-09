<script setup lang="ts">
// The steps for the DAW Plenio runs in (known by the host's name), and for any other DAW to pick.
import { computed, ref } from 'vue'

import { DAWS, dawFor, stepsOf } from '../daws'
import { store } from '../store'
import Icon from './Icon.vue'

const detected = computed(() => dawFor(store.host))
const picked = ref('')
const daw = computed(() => (picked.value === 'other' ? undefined : (DAWS.find((d) => d.id === picked.value) ?? detected.value)))
const steps = computed(() => stepsOf(daw.value))
const choice = computed({
  get: () => picked.value || detected.value?.id || 'other',
  set: (id: string) => (picked.value = id)
})
const title = computed(() => (daw.value ? `In ${daw.value.name}` : 'In your DAW'))
</script>

<template>
  <div class="card guide">
    <div class="card-head">
      <Icon name="library" :size="15" />
      <b>{{ title }}</b>
      <span v-if="daw && daw === detected" class="here">this DAW</span>
      <select v-model="choice" class="select pick" title="The steps for another DAW">
        <option v-for="d in DAWS" :key="d.id" :value="d.id">{{ d.name }}</option>
        <option value="other">Another VST3 DAW</option>
      </select>
    </div>
    <p v-if="daw?.unsupported" class="unsupported"><Icon name="warning" :size="13" />{{ daw.unsupported }}</p>
    <dl v-else class="steps">
      <dt>Add Plenio</dt>
      <dd>{{ steps.add }}</dd>
      <dt>Tempo map</dt>
      <dd>{{ steps.tempo }}</dd>
      <dt>Stems</dt>
      <dd>{{ steps.stems }}</dd>
      <dt>MIDI</dt>
      <dd>{{ steps.midi }}</dd>
      <template v-if="steps.ara">
        <dt>Plenio ARA</dt>
        <dd>{{ steps.ara }}</dd>
      </template>
      <dt>Good to know</dt>
      <dd>{{ steps.audio }}</dd>
    </dl>
  </div>
</template>

<style scoped>
.guide .card-head {
  flex-wrap: wrap;
}
.here {
  font-size: 11px;
  padding: 1px 8px;
  border-radius: 10px;
  background: rgba(47, 195, 168, 0.16);
  color: #6fe0c8;
}
.pick {
  margin-left: auto;
  width: auto;
  min-width: 150px;
  height: 28px;
  font-size: 12px;
  padding: 0 8px;
}
.steps {
  margin: 4px 0 0;
  display: grid;
  grid-template-columns: auto 1fr;
  column-gap: 12px;
  row-gap: 7px;
  font-size: 12.5px;
  line-height: 1.5;
}
.steps dt {
  color: var(--text-faint);
  font-weight: 600;
  white-space: nowrap;
}
.steps dd {
  margin: 0;
  color: var(--text-dim);
}
.unsupported {
  display: flex;
  gap: 7px;
  margin: 4px 0 0;
  font-size: 12.5px;
  line-height: 1.5;
  color: var(--amber);
}
.unsupported :deep(svg) {
  flex: none;
  margin-top: 3px;
}
</style>
