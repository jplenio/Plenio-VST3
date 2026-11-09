# Announcing Plenio VST3 0.1.0-alpha

Texts for every channel, written in the author's voice (drafts: make them yours before posting), and which picture
or video goes with each. Links in angle
brackets are filled in after the upload: `<teaser>` and `<tutorial>` (YouTube), `<release>`
(`https://github.com/jplenio/Plenio-VST3/releases/tag/v0.1.0-alpha`). The files: [launch/README.md](README.md#the-files).

**The idea in one line:** *Generate the song. Keep working in your DAW.*

## The message

**One line (≤ 100 characters):** Plenio VST3: an instrument that writes, composes and sings a song on your own GPU, right in your DAW.

**Short (≤ 200 characters):** Plenio VST3 brings the Plenio Music Production System into your DAW: songs made on your own GPU, up to four takes per run, stems, MIDI and a tempo map that fits. Free and open source.

**Medium:**
Plenio VST3 is a free, open-source VST3 instrument for REAPER, Cubase, Studio One, Ableton Live, FL Studio, Bitwig and
every other VST3 host. Pick a style, write an idea or your own lyrics, and Plenio writes, composes, sings and plays the
song with YuE2 on your own graphics card. Every take lands on your track in time with your project, with its stems, its
MIDI and a tempo map from the bars measured in the take. Change the words, the notes or the chords and render again,
then keep working with your own plug-ins and instruments. REAPER gets the whole song on its own tracks with one click.

**Taglines:** Generate the song. Keep working in your DAW. · Your DAW just learned to write songs. · The first draft is
generated. Everything after it is yours. · Four takes, stems and a tempo map, before your coffee is cold. · No upload. No
export dance. Just your DAW.

**Facts people ask for:** runs locally (ComfyUI + Plenio, YuE2 3B, a 12B writer); 16 GB VRAM is enough; a 1:30 take
takes about two minutes on an RTX 5060 Ti; Windows now, macOS experimental; VST3 + ARA 2; Apache-2.0; the songs are
non-commercial (YuE2 is CC BY-NC 4.0).

## Reddit

Reddit rewards being there: post a **text post** with the video or a picture, on a **weekday morning, US Eastern time**
(15:00 to 17:00 in Germany), and answer every comment in the first two hours. One subreddit a day, each with its own text
(the same text in five places reads as spam). Read each subreddit's rules before posting: self-promotion, AI content,
flair. Say that you are the author, in the first line.

### r/comfyui · first, Plenio's home crowd

**Picture or video:** the teaser (16:9) as a video post, or `docs/media/anim/timelapse.gif`; the link in the first comment
if the subreddit prefers.

**Title:** I turned my ComfyUI music workflow into a VST3: generate a song with YuE2, then keep working on it in your DAW (takes, stems, MIDI, tempo map) [open source]

> Hi all, I'm the author of the Plenio Music Production System. I wanted its songs in a real DAW without a pile of
> exports, so I built **Plenio VST3**: an instrument plug-in that talks to your local ComfyUI through Plenio's Song API.
>
> You put it on a track in REAPER, Cubase, Studio One, Live, FL or Bitwig, pick one of Plenio's 250 styles (or write your
> own brief and lyrics), and ComfyUI does what it always does: the writer drafts the lyrics, YuE2 sings and plays, the
> stems get split, the cover gets painted. What's new is where it lands: right on the track, in time with the project.
> Up to four takes per run, all on the same track. Every bar is measured in the take, so the tempo map you drop into
> your DAW actually fits the song, not just its average BPM.
>
> Then you edit: change a line of the lyrics and the next take sings it (with new notes for the new words), move notes in
> the piano roll or change a chord and it sings exactly that. In REAPER it lays everything out on the project's own
> tracks, every take as one of REAPER's takes.
>
> It needs Plenio 1.6 (the new Song API with stems and bars), runs on 16 GB, Windows now, macOS experimental. Free,
> Apache-2.0. YuE2 is CC BY-NC 4.0, so the songs are non-commercial.
>
> Teaser: `<teaser>` · Tutorial: `<tutorial>` · Repo and download: https://github.com/jplenio/Plenio-VST3
>
> I'd love to hear what breaks on your setup, and which DAW you'd want it tested in next.

### r/Reaper · the deepest integration, the most demanding crowd

**Picture or video:** `docs/media/screens/reaper-tracks.png` (the song on REAPER's tracks) as the post's image, or
`docs/media/anim/reaper-tracks.gif` (the click on *Put on tracks* and the tracks arriving); the tutorial video in the
text. Lead with REAPER, not with AI.

**Title:** I built a free VST3 that lays a generated song out on REAPER's own tracks: every take as a REAPER take, stems, MIDI, lyrics as items, sections as regions, and a tempo map from the measured bars

> REAPER user here, and the author of this plug-in, so take my enthusiasm with a grain of salt.
>
> Plenio VST3 is an instrument that makes songs locally (YuE2 through ComfyUI, on your own GPU). That part works in any
> DAW. But REAPER's API let me go a lot further, and I think some of you will enjoy the details:
>
> - **Put on tracks** builds a folder: Lyrics (an empty item per sung line, the line as the item's notes), Song (every
>   take as one of REAPER's takes on one item), Melody / Instrument Line / Chords as MIDI (muted, ready for your own
>   instruments), and the stems.
> - The **tempo map** comes from the bars as they were measured in the take, written with `SetTempoTimeSigMarker`, so the
>   grid follows the drift of a generated song.
> - Choose a take on the take lane (or press T): the plug-in follows, and the stems, MIDI, lyrics and tempo map follow the
>   take. Choose one in the plug-in: REAPER follows.
> - The plug-in's own audio items get timebase **Time**, so importing a tempo map moves the grid, not the song (found
>   the hard way: the default beats timebase stretched my first test to 63 % speed).
> - The cover art becomes the tracks' icon, and space plays and stops REAPER even while the plug-in has focus.
> - There's an ARA 2 part too: put Plenio ARA on a take's item and REAPER gets the take's tempo, key, notes and chords.
>
> Free and open source (Apache-2.0), Windows, 0.1.0-alpha. The songs themselves are non-commercial (the music model is
> CC BY-NC 4.0).
>
> Tutorial (in REAPER): `<tutorial>` · Repo: https://github.com/jplenio/Plenio-VST3
>
> If you know a cleaner way to do any of the API bits, I'm all ears.

### r/LocalLLaMA · fully local, numbers welcome

**Picture or video:** `docs/media/anim/timelapse.gif` (the takes being made) and the facts.

**Title:** Fully local AI songwriting inside any DAW: YuE2 + a 12B writer on one 16 GB GPU, packed as a VST3 (open source)

> The whole chain runs on my own machine: Gemma 4 12B (GGUF, llama.cpp) drafts the lyrics and the plan, YuE2 (3B) sings
> and plays, SheetSage2 transcribes the melody into a score, BS-RoFormer splits stems, FLUX.2 Klein 4B paints a cover. All
> of it orchestrated by ComfyUI, and now driven from a VST3 instrument inside the DAW, so the result lands as takes,
> stems and MIDI on your tracks instead of a folder of files.
>
> Numbers from my box (RTX 5060 Ti 16 GB): a 1:30 take in about two minutes, four takes per run, stems about 30 s extra
> per take. ComfyUI handles the offloading, the writer server is gone when the draft is done.
>
> Plenio VST3 is free and Apache-2.0; YuE2 is CC BY-NC 4.0, so the songs are non-commercial.
>
> https://github.com/jplenio/Plenio-VST3 · Teaser: `<teaser>`

### r/AudioPlugins · a new free instrument

**Picture or video:** `docs/media/screens/song.png`, the teaser in the text.

**Title:** Plenio VST3 (free, open source): an instrument that writes, composes and sings songs on your own GPU, then hands you the takes, stems and MIDI

> I'm the developer. Plenio VST3 is a VST3 instrument (plus an ARA 2 extension) that generates whole songs locally and
> keeps them editable: up to four takes per run on one track, five stems per take on extra outputs, melody / instrument
> line / chords as MIDI out, and drag-and-drop files (WAV, MIDI with the song's tempo of every bar, a tempo map, a click).
> Change the lyrics, notes or chords inside the plug-in and render again.
>
> It needs ComfyUI with Plenio on the same machine (that's where the models run). Windows now, macOS experimental,
> Apache-2.0. The songs are for non-commercial use (YuE2's licence).
>
> Teaser: `<teaser>` · Download: `<release>`

### r/WeAreTheMusicMakers · only where the rules allow it

WATMM is strict about self-promotion and wary of AI. Post only in the thread the rules name for it (or ask the mods
first), and lead with the question, not the product.

**Picture or video:** `docs/media/anim/notes.gif` (a human moving notes): the point is the control.

**Title:** I built a tool where a local AI only does the first draft of a song, and everything after it happens in your DAW. Where would you draw the line?

> I make music in REAPER and I've been building a plug-in that drafts a song (lyrics, melody, chords, a sung demo) on
> my own computer and then gets out of the way: the takes, stems and MIDI land on my tracks, and from
> there it's my instruments, my edits, my mix. I can change a line and have it sung again, or move notes and hear exactly
> those.
>
> For me it replaced the blank page, not the work. But I know a lot of people here feel strongly about AI in music, and
> I'd honestly like to hear where you'd draw the line: is a generated draft you then rewrite and re-arrange still "your"
> song?
>
> (The tool is free and open source, if anyone wants to look: https://github.com/jplenio/Plenio-VST3. The songs are
> non-commercial because of the model's licence.)

### r/opensource · how it is built

**Picture or video:** `docs/media/social-preview.png`.

**Title:** Plenio VST3: an open-source VST3 + ARA 2 plug-in that brings a local AI music system into any DAW (C++20, Vue in a WebView, Apache-2.0)

> I've just released the first alpha of Plenio VST3. Some things that might interest people who build plug-ins: C++20
> on the VST3 SDK 3.8 (now MIT) and the ARA 2 SDK; the UI is Vue 3 in a WebView (choc), talking JSON to the plug-in; the
> heavy lifting happens in ComfyUI over a small HTTP API; REAPER's extension API through `IReaperHostApplication`, tested
> against a fake REAPER in the unit tests; GitHub Actions build Windows with the VST3 validator, Linux and an experimental
> macOS bundle, and a weekly job checks every new Plenio release against what the plug-in uses and opens a PR.
>
> https://github.com/jplenio/Plenio-VST3. Feedback and DAW reports very welcome.

### Later: the DAW subreddits

r/cubase, r/StudioOne, r/ableton, r/FL_Studio, r/Bitwig: post there once someone has tried Plenio in that DAW (the
first issue reports will tell), with that DAW's screenshot and its own steps from [DAWS.md](../DAWS.md).

## YouTube

### The teaser

**Title:** Plenio VST3: generate the song, keep working in your DAW

**Description:**

> Plenio VST3 brings the Plenio Music Production System into your DAW: songs made on your own GPU, up to four takes per
> run, stems, MIDI and a tempo map that fits. Then change the words, the notes or the chords and render again, and keep
> working with your own plug-ins and instruments.
>
> Free and open source (Apache-2.0): https://github.com/jplenio/Plenio-VST3
> Tutorial: `<tutorial>`
>
> The music in this video was made with Plenio (YuE2, CC BY-NC 4.0).

**Tags:** Plenio, VST3, AI music, songwriting, REAPER, ComfyUI, YuE, local AI, music production, DAW, open source

### The tutorial

**Title:** Plenio VST3 tutorial: your first song in your DAW, step by step (REAPER)

**Description:**

> Your first song with Plenio VST3, from installing to the song on REAPER's tracks: a style, an idea, takes and stems,
> changing the words and the notes, the files for your project, and what other DAWs do differently.
>
> Written tutorial: https://github.com/jplenio/Plenio-VST3/blob/main/docs/TUTORIAL.md
> Every DAW's steps: https://github.com/jplenio/Plenio-VST3/blob/main/docs/DAWS.md
> Download: `<release>` · Teaser: `<teaser>`
>
> Chapters: (from launch/README.md, after the cut)
>
> The music in this video was made with Plenio (YuE2, CC BY-NC 4.0).

## Instagram, TikTok, YouTube Shorts · the vertical teaser

**Caption:**

> Your DAW just learned to write songs. 🎛️ Pick a style, Plenio writes, composes and sings it on your own GPU, and every
> take lands on your track, with stems and MIDI. Then it's yours. Free and open source: link in bio.
>
> #musicproduction #daw #songwriting #aimusic #reaper #homestudio #producer #vst #opensource #comfyui

Hooks for the first second (on the video or as the caption's first line): *"This plug-in wrote a song while I made coffee."* ·
*"Four takes, stems and a tempo map. One click."* · *"POV: your DAW writes the first draft."*

## LinkedIn

> Today I released the first alpha of **Plenio VST3**, an open-source plug-in that brings my Plenio Music Production
> System into the tools musicians already work in: REAPER, Cubase, Studio One, Ableton Live, FL Studio, Bitwig.
>
> The idea: generative AI should hand over editable material, not finished files. Plenio writes, composes and sings a
> song on your own GPU, and every take arrives on your DAW's track with its stems, its MIDI and a tempo map measured in the
> audio. Change the words or the notes and it renders again; everything after the first draft happens with your own
> instruments and plug-ins.
>
> Under the hood: C++20 on the VST3 and ARA 2 SDKs, a Vue UI in a WebView, ComfyUI as the engine, and CI that checks every
> new release of the AI system against the plug-in automatically.
>
> Free, Apache-2.0: https://github.com/jplenio/Plenio-VST3
>
> #MusicTechnology #GenerativeAI #OpenSource #AudioPlugins #MusicProduction

**Picture:** `docs/media/social-preview.png`, or the teaser as a native video.

## Bluesky and Mastodon

> Plenio VST3 is out (0.1.0-alpha): an open-source instrument that writes, composes and sings a song on your own GPU,
> right in your DAW. Up to four takes per run, stems, MIDI, a tempo map that fits. Then you take over. 🎶
> https://github.com/jplenio/Plenio-VST3
>
> #AudioPlugins #MusicProduction #OpenSource #AIMusic #REAPER

**Picture:** `docs/media/anim/timelapse.gif` (Mastodon: with an alt text, e.g. "A song being made in the Plenio VST3
plug-in: two takes arrive with lyrics, notes and cover art").

## Hacker News

**Title:** Show HN: Plenio VST3 – a VST3 that generates songs locally and hands you editable takes

> Hi HN, I built Plenio VST3, an open-source VST3 instrument (plus an ARA 2 extension) that drives a local AI music
> pipeline (ComfyUI with YuE2 for vocals and music, a 12B writer for lyrics, stem separation) and puts the results into
> the DAW as editable material: takes on one track, stems on extra outputs, the melody and chords as MIDI, and a tempo map
> built from bars measured in the generated audio. In REAPER it writes the song onto the project's tracks through
> REAPER's extension API.
>
> Some details I enjoyed: the UI is Vue in a WebView talking JSON to the C++ plug-in; the REAPER code is unit-tested
> against a fake REAPER that stretches items the way REAPER's timebase does; and a weekly CI job checks each new release
> of the AI system against everything the plug-in relies on and opens a PR. Windows now, macOS experimental. The songs are
> non-commercial (the music model is CC BY-NC 4.0).
>
> https://github.com/jplenio/Plenio-VST3

## Discord · ComfyUI, YuE, REAPER and home-studio servers (#showcase, #projects)

> 👋 I just released **Plenio VST3** (alpha): Plenio's songs, right in your DAW. Pick a style, ComfyUI makes up to four
> takes with stems, and they land on your track in time with the project, with MIDI and a tempo map. Change words,
> notes or chords and render again. Free, open source: https://github.com/jplenio/Plenio-VST3 · teaser `<teaser>`

**Attach:** the 720p teaser (under 10 MB, plays in Discord).

## Facebook groups · REAPER users, home studio, AI music

> I've released a free, open-source plug-in I've been building: **Plenio VST3**. It makes songs on your own computer
> (lyrics, melody, a sung demo) and drops them into your DAW as takes, stems and MIDI, with a tempo map that fits. Then
> you keep working with your own tools. Tutorial and download in the comments. Happy about every test report!

**Attach:** the vertical teaser; the links in the first comment (groups often bury posts with links).

## KVR Audio · Developer news and the "Instruments" forum

**Subject:** Plenio VST3 0.1.0-alpha: free, open-source song generator instrument (VST3, ARA 2)

> Plenio VST3 is a free, open-source VST3 instrument that writes, composes and sings songs locally (with ComfyUI and the
> Plenio Music Production System on your own GPU) and keeps them editable in the DAW: up to four takes per run, stems on
> extra outputs, MIDI out, drag-and-drop WAV/MIDI/tempo map/click, lyrics, notes and chords to change and render again.
> Deep REAPER integration; an ARA 2 extension for clips.
>
> Formats: VST3 (Windows x64; macOS universal, experimental). Licence: Apache-2.0; the songs are non-commercial (YuE2,
> CC BY-NC 4.0). https://github.com/jplenio/Plenio-VST3

## Product Hunt

**Tagline:** Generate the song. Keep working in your DAW.
**Description:** the medium text above. **Gallery:** `social-preview.png`, `song.png`, `reaper-song.png`,
`reaper-tracks.png`, `daw-guide.png`, the teaser. **First comment:** why it exists (the blank page, editable
material instead of files) and what to test.

## Plenio's own channels

- The Plenio Music Production System's README: a line under its integrations, *Plenio VST3: Plenio in your DAW*.
- Plenio's Discussions (or its release notes for 1.6.0): *Plenio 1.6 brings the Song API that Plenio VST3 uses: stems
  with every take, the bars of a take.*

## GitHub

**Description:** The Plenio Music Production System in your DAW: songs, takes, stems, lyrics, melody and chords, made on
your own GPU. VST3 + ARA 2.

**Website:** https://github.com/jplenio/Plenio-VST3#readme

**Topics:** `vst3` `vst3-plugin` `ara2` `audio-plugin` `daw` `reaper` `music-production` `ai-music` `songwriting`
`comfyui` `yue` `generative-ai` `local-ai` `midi` `stems` `cpp20` `vue`

**Social preview:** `docs/media/social-preview.png`.

## Which file for what

| Post | Picture or video |
|---|---|
| r/comfyui, Discord, YouTube | the teaser (16:9; Discord: the 720p copy) |
| r/Reaper, Product Hunt | `docs/media/screens/reaper-tracks.png` or `docs/media/anim/reaper-tracks.gif`, the tutorial |
| r/LocalLLaMA, Bluesky, Mastodon | `docs/media/anim/timelapse.gif` |
| r/AudioPlugins | `docs/media/screens/song.png` |
| r/WeAreTheMusicMakers | `docs/media/anim/notes.gif` |
| r/opensource, LinkedIn, Hacker News | `docs/media/social-preview.png` |
| Instagram, TikTok, Shorts, Facebook | the vertical teaser (9:16) |
| Release text | the 720p teaser in place, the YouTube links |
