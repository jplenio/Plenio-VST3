// What the plug-in sends the UI (src/plugin/song_controller.cpp: songView, connectionView, appState).

export type Note = [tick: number, length: number, pitch: number, velocity: number]
export type Chord = [tick: number, name: string]

export interface Connection {
  state: 'searching' | 'connected' | 'offline' | 'starting'
  host: string
  port: number
  plenio: string
  api: string
  comfyui: string
  features: string[]
  message: string
  startComfyUI?: boolean
}

export interface Settings {
  library: string
  port: number
  startComfyUI: boolean
  licenceAccepted: boolean
  takesPerRun: number
  stems: boolean
  flatKeys: boolean
}

export interface TakeView {
  index: number
  number: number
  title: string
  label: string
  stars?: number
  seconds: number
  created: number
  what: string
  stems: string[]
  hasAudio: boolean
  measured: boolean
  audioAt?: number
  daw?: '' | 'preparing' | 'ready' | 'failed'
}

export interface DawView {
  number: number
  state: '' | 'preparing' | 'ready' | 'failed'
  problem?: string
  stems?: string[]
  click?: boolean
  folder?: string
}

export interface JobView {
  number: number
  index: number
  status: string
  stage: string
  progress: number
  what: string
  queuePosition?: number
}

export interface ScoreView {
  ppq: number
  bpm: number
  key: string
  endTick: number
  meters: [tick: number, numerator: number, denominator: number][]
  sections: [tick: number, name: string][]
  melody: Note[]
  instrument: Note[]
  chords: Chord[]
}

export interface Measure {
  tick: number
  endTick: number
  start: number
  end: number
  number: number
}

export interface Line {
  start: number
  end: number
  text: string
}

export interface SongView {
  id: string
  title: string
  kind: 'song' | 'cover'
  template: string
  fields: Record<string, unknown>
  documents: Record<string, { text: string; manual: boolean }>
  takesPerRun: number
  stems: boolean
  activeTake: number
  changes: string[]
  plan: { what: string; label: string }
  edited: { melody: boolean; instrument: boolean; chords: boolean }
  loading: boolean
  loadProblem: string
  folder: string
  reaper?: { available: boolean; placed: boolean }
  takes: TakeView[]
  jobs: JobView[]
  layout?: { scoreAt: number; audioAt: number; leadInBars: number; measured: boolean; seconds: number; tempo: [number, number] }
  lines?: Line[]
  score?: ScoreView
  measures?: Measure[]
  leadIn?: { start: number; end: number; number: number }[]
}

export interface AppState {
  hello: { kind: string; version: string; host: string }
  connection: Connection
  settings: Settings
  song: SongView | null
  position: number
}

/** A field of a template (GET /plenio/api/v1/templates/{id}). */
export interface Field {
  id: string
  label: string
  type: 'text' | 'string' | 'integer' | 'number' | 'boolean' | 'choice' | 'upload'
  default: unknown
  description?: string
  choices?: string[]
  minimum?: number
  maximum?: number
  step?: number
  when?: Record<string, string[]>
  fixed?: boolean
  required?: boolean
  advanced?: boolean
  group?: string
}

export interface TemplateDetail {
  id: string
  title: string
  kind: string
  fields: Field[]
}

export interface Brief {
  id: string
  name: string
  group?: string
  fields?: Record<string, unknown>
}

export interface LibrarySong {
  id: string
  title: string
  kind: string
  takes: number
  modified: number
  stars?: number
}
