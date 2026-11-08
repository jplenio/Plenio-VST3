<script setup lang="ts">
import { computed, nextTick, ref } from 'vue'

import { act, store } from '../store'
import Icon from './Icon.vue'

defineEmits<{ library: []; settings: []; new: [] }>()

const editing = ref(false)
const title = ref('')
const field = ref<HTMLInputElement | null>(null)

const status = computed(() => {
  const c = store.connection
  if (!c) return { tone: 'wait', text: '…' }
  if (c.state === 'connected') return { tone: 'ok', text: `Plenio ${c.plenio}` }
  if (c.state === 'starting') return { tone: 'wait', text: 'Starting ComfyUI …' }
  if (c.state === 'searching') return { tone: 'wait', text: 'Looking for ComfyUI …' }
  return { tone: 'off', text: 'Studio offline' }
})

async function rename(): Promise<void> {
  if (!store.song) return
  editing.value = true
  title.value = store.song.title
  await nextTick()
  field.value?.select()
}

async function commit(): Promise<void> {
  if (!editing.value) return
  editing.value = false
  const t = title.value.trim()
  if (store.song && t && t !== store.song.title) await act('song.rename', { title: t })
}
</script>

<template>
  <header class="bar">
    <div class="brand">
      <img src="/plenio-icon.png" alt="" class="logo" />
      <span class="name">Plenio</span>
    </div>
    <div v-if="store.song" class="song">
      <span class="sep">/</span>
      <input
        v-if="editing"
        ref="field"
        v-model="title"
        class="input title-edit"
        @keydown.enter="commit"
        @keydown.esc="editing = false"
        @blur="commit"
      />
      <button v-else class="title" title="Rename the song" @click="rename">
        {{ store.song.title }}
        <Icon name="pen" :size="13" class="pen" />
      </button>
      <span v-if="store.song.kind === 'cover'" class="kind">cover</span>
    </div>
    <div class="spacer" />
    <div class="status" :class="status.tone" :title="store.connection?.message">
      <span class="light" />
      {{ status.text }}
    </div>
    <button class="btn ghost small" title="New song" @click="$emit('new')"><Icon name="plus" :size="15" />New</button>
    <button class="btn ghost small" title="Your songs" @click="$emit('library')"><Icon name="library" :size="15" />Songs</button>
    <button class="btn ghost small icon" title="Settings" @click="$emit('settings')"><Icon name="gear" :size="16" /></button>
  </header>
</template>

<style scoped>
.bar {
  height: 50px;
  flex: none;
  display: flex;
  align-items: center;
  gap: 8px;
  padding: 0 14px 0 12px;
  border-bottom: 1px solid var(--line);
  background: rgba(18, 16, 25, 0.75);
}
.brand {
  display: flex;
  align-items: center;
  gap: 9px;
}
.logo {
  width: 26px;
  height: 26px;
  border-radius: 7px;
}
.name {
  font-family: var(--font-display);
  font-weight: 650;
  font-size: 16px;
  letter-spacing: 0.01em;
}
.song {
  display: flex;
  align-items: center;
  gap: 8px;
  min-width: 0;
}
.sep {
  color: var(--text-faint);
  font-size: 18px;
}
.title {
  display: inline-flex;
  align-items: center;
  gap: 7px;
  border: none;
  background: transparent;
  font-size: 15px;
  font-weight: 600;
  cursor: text;
  padding: 4px 6px;
  border-radius: 6px;
  max-width: 340px;
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
}
.title:hover {
  background: var(--bg-hover);
}
.pen {
  color: var(--text-faint);
  opacity: 0;
}
.title:hover .pen {
  opacity: 1;
}
.title-edit {
  width: 260px;
  height: 30px;
  font-size: 15px;
  font-weight: 600;
}
.kind {
  font-size: 11px;
  padding: 2px 8px;
  border-radius: 10px;
  background: rgba(255, 178, 62, 0.14);
  color: var(--amber);
}
.spacer {
  flex: 1;
}
.status {
  display: flex;
  align-items: center;
  gap: 7px;
  height: 26px;
  padding: 0 11px;
  border-radius: 13px;
  background: var(--bg-raised);
  border: 1px solid var(--line);
  font-size: 12px;
  color: var(--text-dim);
  margin-right: 6px;
}
.light {
  width: 7px;
  height: 7px;
  border-radius: 50%;
  background: var(--text-faint);
}
.status.ok .light {
  background: var(--ok);
  box-shadow: 0 0 8px rgba(79, 209, 165, 0.7);
}
.status.wait .light {
  background: var(--wait);
  animation: pulse 1.2s infinite;
}
.status.off .light {
  background: #7d7690;
}
</style>
