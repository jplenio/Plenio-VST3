<script setup lang="ts">
import { computed, onBeforeUnmount, onMounted, ref } from 'vue'

import { act, jobText, online, store } from '../store'
import { formatTime } from '../timeline'
import Icon from './Icon.vue'

const menu = ref(false)
const song = computed(() => store.song!)
const active = computed(() => song.value.takes[song.value.activeTake])
const busy = computed(() => song.value.jobs.length > 0)

const facts = computed(() => {
  const out: string[] = []
  const take = active.value
  if (take) {
    out.push(`Take ${take.number} of ${song.value.takes.length}`)
    out.push(formatTime(take.seconds))
  }
  const tempo = song.value.layout?.tempo
  if (tempo) {
    const [lo, hi] = tempo
    out.push(Math.abs(hi - lo) < 0.6 ? `${lo.toFixed(1)} BPM` : `${lo.toFixed(1)}–${hi.toFixed(1)} BPM`)
  }
  if (song.value.score?.key) out.push(song.value.score.key)
  return out
})

const generateLabel = computed(() => {
  const n = song.value.takesPerRun
  if (song.value.takes.length === 0) return n > 1 ? `Make ${n} takes` : 'Make the song'
  return n > 1 ? `Generate ${n} takes` : 'Generate a take'
})

const changeNames: Record<string, string> = {
  brief: 'brief changed',
  lyrics: 'lyrics changed',
  title: 'title changed',
  style: 'style changed',
  score: 'score changed',
  notes: 'notes changed',
  chords: 'chords changed',
  'artwork prompt': 'artwork changed'
}

function generate(mode = 'auto'): void {
  menu.value = false
  void act('song.generate', { mode })
}


function closeMenu(e: MouseEvent): void {
  if (!(e.target as HTMLElement).closest('.generate')) menu.value = false
}
onMounted(() => document.addEventListener('mousedown', closeMenu))
onBeforeUnmount(() => document.removeEventListener('mousedown', closeMenu))
</script>

<template>
  <section class="header">
    <div class="facts">
      <div class="line">
        <template v-if="active">
          <span v-if="active.stars !== undefined" class="stars" :title="`Plenio's measured quality: ${active.stars} of 5`">
            <Icon name="star" :size="13" />{{ active.stars }}
          </span>
          <span v-for="f in facts" :key="f" class="fact">{{ f }}</span>
        </template>
        <span v-else-if="!busy" class="muted">No take yet</span>
        <span v-for="c in song.changes" :key="c" class="chip">{{ changeNames[c] ?? `${c} changed` }}</span>
      </div>
      <div v-if="busy" class="jobs">
        <div v-for="job in song.jobs" :key="job.number" class="job" :class="job.status">
          <span class="spinner" v-if="job.status === 'running' || job.status === 'submitting'" />
          <span class="take">Take {{ job.number }}</span>
          <span class="muted">{{ jobText(job, song.jobs) }}</span>
          <span v-if="job.status === 'running'" class="bar"><span :style="{ width: `${Math.round(job.progress * 100)}%` }" /></span>
        </div>
      </div>
      <div v-else-if="song.loadProblem" class="problem"><Icon name="warning" :size="13" />{{ song.loadProblem }}</div>
    </div>

    <div class="make">
      <div class="options">
        <div class="segmented" title="How many takes one run makes">
          <button v-for="n in 4" :key="n" :class="{ on: song.takesPerRun === n }" @click="act('song.takesPerRun', { count: n })">{{ n }}</button>
        </div>
        <label class="toggle" title="Split every take into vocals, drums, bass, other and the rest">
          <input type="checkbox" :checked="song.stems" @change="act('song.stems', { on: ($event.target as HTMLInputElement).checked })" />
          <span class="track" />stems
        </label>
      </div>
      <div class="generate">
        <button v-if="busy" class="btn" @click="act('song.cancel')"><Icon name="stop" :size="12" />Cancel</button>
        <template v-else>
          <button class="btn primary go" :disabled="!online()" @click="generate()">
            <Icon name="sparkle" :size="16" />{{ generateLabel }}
          </button>
          <button class="btn primary more" :disabled="!online() || song.takes.length === 0" @click="menu = !menu">
            <Icon name="chevron-down" :size="14" />
          </button>
        </template>
        <div v-if="menu" class="menu">
          <button @click="generate('newTake')"><b>New take</b><span>the same words and notes, sung again</span></button>
          <button @click="generate('recompose')"><b>Compose again</b><span>the same words, new notes</span></button>
          <button @click="generate('rewrite')"><b>Write anew</b><span>a new song from the brief</span></button>
        </div>
      </div>
      <div class="plan muted">{{ online() ? song.plan.label : 'start ComfyUI to make takes' }}</div>
    </div>
  </section>
</template>

<style scoped>
.header {
  flex: none;
  display: flex;
  align-items: flex-start;
  gap: 20px;
  padding: 12px 16px 12px;
  border-bottom: 1px solid var(--line);
  background: rgba(22, 19, 31, 0.6);
}
.facts {
  flex: 1;
  min-width: 0;
  display: flex;
  flex-direction: column;
  gap: 8px;
}
.line {
  display: flex;
  align-items: center;
  gap: 8px;
  flex-wrap: wrap;
  min-height: 26px;
}
.stars {
  display: inline-flex;
  align-items: center;
  gap: 4px;
  height: 24px;
  padding: 0 9px;
  border-radius: 12px;
  background: rgba(255, 178, 62, 0.12);
  color: var(--amber);
  font-weight: 650;
}
.fact {
  color: var(--text);
  font-weight: 550;
}
.fact + .fact::before {
  content: '·';
  margin-right: 8px;
  color: var(--text-faint);
}
.jobs {
  display: flex;
  flex-direction: column;
  gap: 5px;
}
.job {
  display: flex;
  align-items: center;
  gap: 9px;
  font-size: 12.5px;
}
.job .take {
  font-weight: 600;
  width: 56px;
}
.bar {
  width: 120px;
  height: 4px;
  border-radius: 2px;
  background: #2c2640;
  overflow: hidden;
}
.bar span {
  display: block;
  height: 100%;
  background: var(--gradient);
  transition: width 0.4s;
}
.spinner {
  width: 12px;
  height: 12px;
  border-radius: 50%;
  border: 2px solid rgba(176, 92, 255, 0.3);
  border-top-color: var(--accent);
  animation: spin 0.8s linear infinite;
}
.job.queued::before {
  content: '';
  width: 12px;
  height: 12px;
  border-radius: 50%;
  border: 2px dashed #5a5070;
}
.problem {
  display: flex;
  align-items: center;
  gap: 6px;
  color: var(--amber);
  font-size: 12.5px;
}
.make {
  display: grid;
  grid-template-columns: auto auto;
  grid-template-rows: auto auto;
  column-gap: 14px;
  row-gap: 6px;
  align-items: center;
  justify-items: end;
}
.options {
  display: flex;
  align-items: center;
  gap: 14px;
}
.generate {
  position: relative;
  display: flex;
}
.go {
  height: 38px;
  padding: 0 18px;
  font-size: 14px;
  border-radius: 9px 0 0 9px;
}
.more {
  height: 38px;
  width: 34px;
  padding: 0;
  border-radius: 0 9px 9px 0;
  border-left: 1px solid rgba(255, 255, 255, 0.2) !important;
}
.generate > .btn:only-child {
  height: 38px;
  border-radius: 9px;
}
.plan {
  grid-column: 1 / 3;
  font-size: 12px;
}
.menu {
  position: absolute;
  top: 44px;
  right: 0;
  z-index: 20;
  width: 280px;
  padding: 6px;
  border-radius: 11px;
  background: #1f1a2c;
  border: 1px solid var(--line-strong);
  box-shadow: var(--shadow);
  display: flex;
  flex-direction: column;
}
.menu button {
  display: flex;
  flex-direction: column;
  align-items: flex-start;
  gap: 2px;
  padding: 9px 11px;
  border: none;
  border-radius: 8px;
  background: transparent;
  cursor: pointer;
  text-align: left;
}
.menu button:hover {
  background: var(--bg-hover);
}
.menu span {
  color: var(--text-dim);
  font-size: 12px;
}
</style>
