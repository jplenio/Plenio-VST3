# Testing Plenio VST3 (test version 0.1.0-alpha)

What this test version does, and how to try every part of it in REAPER and Cubase. Report what you find (see the
end); the alpha release follows from it.

## What you need

- Windows 10 or 11 (x64) with the Microsoft Edge WebView2 Runtime (Windows 11 has it; `tools\install.ps1` checks).
- Your ComfyUI with Plenio, started as you always start it. Plenio VST3 finds it on port 8188, 8000 or the port of
  `~\.plenio\comfyui-start.json`, and can start it from there when it is not running (Settings).
- **Plenio 1.6.0** (Song API 1.4, branch `song-api-1.4` of the Plenio repository) for stems with every take and the
  bars measured in the take. Your ComfyUI has an older Plenio (`custom_nodes\Plenio-Music-Production-System` at
  `58b6a94`): songs are made with it too, but without stems, and the tempo map follows Plenio's rough estimate. To
  give your ComfyUI 1.6.0 (a fast-forward, nothing of yours changes), then restart ComfyUI:

  ```powershell
  cd D:\Daten2\ComfyUI\custom_nodes\Plenio-Music-Production-System
  git fetch D:\Daten2\Deepseek\ComfyUI-MiniMax\Plenio-Music-Production-System song-api-1.4
  git merge --ff-only FETCH_HEAD
  ```

## Install

1. Build (if `build\VST3\Release\Plenio.vst3` is not there yet): `tools\build.ps1`.
2. `tools\install.ps1`: asks for administrator rights and copies `Plenio.vst3` into
   `C:\Program Files\Common Files\VST3`, where every host looks. (`tools\install.ps1 -Uninstall` takes it out again.)
3. Let the DAW scan for new plug-ins. Two plug-ins appear: **Plenio** (an instrument) and **Plenio ARA** (an ARA
   extension for clips).

Plenio keeps your songs in `Documents\Plenio\Songs`, its settings in `%APPDATA%\Plenio\VST3` and its log in
`%LOCALAPPDATA%\Plenio\VST3\plenio.log`.

## In REAPER

1. **Insert > Virtual instrument on new track… > VST3i: Plenio (Plenio).** The window shows the studio's state at the
   top right (green: *Plenio 1.x*). Before the first song it asks you to accept the models' licences (YuE2 and
   SheetSage2: CC BY-NC 4.0).
2. **New song**: a style on the left fills the brief (change any field), *Length: very short*, *Takes: 2*, *stems* on
   (with 1.6.0). *Make 2 takes*. A take takes about two minutes; both land on the take lanes.
3. **Play** (the space bar works in Plenio's window too): the instrument plays the chosen take in time with REAPER.
   Click Plenio's ruler while REAPER stands to listen from there. Choose the other take: it plays instead.
4. **DAW tab > Put on tracks**: below the instrument a folder *Plenio · <title>* with *Lyrics* (an item per sung
   line), *Song* (every take as one of REAPER's takes), *Melody*, *Instrument Line*, *Chords* (muted MIDI) and the
   stems; the sections as regions; the project's tempo map from the take's bars. Check:
   - REAPER's bar lines fall on the song's bars (the click of the DAW tab helps hearing it);
   - choose another take on the Song track (its take lane, or the *T* key): Plenio follows, and the stems, MIDI, lyrics
     and tempo map follow the take; choose one in Plenio: REAPER follows;
   - the instrument is quiet while REAPER plays the tracks (it still plays when you listen inside Plenio).
5. **Change the song**:
   - *Words* tab: change a line of the lyrics. *Generate* now says *compose new notes for the words, then render*.
   - the piano roll (double-click: a note; drag: move; its edge: length; Del; arrow keys: transpose), and a chord
     (click it). *Generate* says *render a take of your notes*.
   - *Brief* tab: change the brief: the song is written anew.
   Each run lands as new takes on the same track, also when the new take is longer or shorter.
6. **Drag and drop** (DAW tab, or the grip on a take lane): drag *Take*, *Stems*, *MIDI*, *Tempo map* or *Click* onto
   REAPER's timeline at bar 1. Every file starts at the project's start.
7. **Without ComfyUI**: save the project, quit REAPER, quit ComfyUI, open the project again. The song, its takes and
   your edits are there; playing, editing, dragging and *Put on tracks* work; *Generate* asks for ComfyUI.

## In Cubase

1. An instrument track with **Plenio**: it plays the song in time with Cubase.
2. The stems on their own channels: activate the outputs *Vocals*, *Drums*, *Bass*, *Other*, *Rest* in the
   instrument's output list (VST Instruments rack); an active stem output leaves the main output.
3. *MIDI out*: the melody (channel 1), the instrument line (2) and the chords (3) go to a MIDI track routed from Plenio.
4. Drag and drop as in REAPER; the tempo map: drop *Tempo map* or use *File > Import > MIDI File…* with the tempo
   track imported.

## Plenio ARA (Cubase, Studio One, REAPER)

1. Drag a take (*Take* tile of the DAW tab) into the project and put **Plenio ARA** on that clip (Cubase: select the
   event, *Audio > Extensions > Plenio ARA*; REAPER: the item's FX, Plenio ARA).
2. Plenio ARA knows the clip as a Plenio take (the log says `ARA: '<file>' is take <n> of <title>`) and hands the host
   the take's tempo of every bar, meter, key, sung notes and chords. Hosts that read ARA content show them (Studio One:
   chord track and tempo from the clip; Cubase and REAPER: tell me what you see).
3. The clip plays unchanged through Plenio ARA.
4. Open Plenio ARA's window: it lists the clips that are Plenio takes. Choose another take of the song: after a moment
   it plays in the same clip, at the same places (a stem's clip plays the other take's same stem). The project's tempo
   map stays the one of the clip's take.

## Not in this version

- More ARA: the take's lyrics, notes and chords in Plenio ARA's window, covers of any clip.
- The syllables under the notes (Plenio's MIDI export has no syllables yet: API addition A6), undo and redo across all
  edits (the piano roll has its own), notes edited in REAPER read back, macOS and Linux.

## What to tell me

- What did not work or looked wrong: the DAW and its version, what you did, a screenshot.
- The log: `%LOCALAPPDATA%\Plenio\VST3\plenio.log`.
- What you would like to work differently.
