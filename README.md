# Plenio VST3

**Write the words. Generate the song. Edit every take - in your DAW.**

The [Plenio Music Production System](https://github.com/jplenio/Plenio-Music-Production-System) as a VST3 plug-in:
songs and covers made with YuE2 on your own graphics card, with up to four takes per run, stems, and the melody,
chords and lyrics to change by hand and render again.

> **Status: in development** (0.1.0-alpha). The plan and its progress: [docs/PLAN.md](docs/PLAN.md).

## Two plug-ins in one bundle

| Plug-in | Type | What it does |
|---|---|---|
| **Plenio** | VST3 instrument | One song: new song or cover, takes (up to 4 per run), stems on extra outputs, melody, chords and lyrics to edit, played in sync with your DAW; drag takes, stems and MIDI with the song's tempo map into your project. In REAPER it puts the song on tracks itself, every take as a REAPER take. |
| **Plenio ARA** | ARA 2 extension | On a clip in Cubase, Studio One, REAPER and other ARA hosts: knows Plenio's takes, hands the host their tempo, bars, key, sung notes and chords, and plays any other take of the song in the same clip. *Next:* the take's lyrics, notes and chords in its window, covers of any clip. |

## Requirements

- Windows 10 or 11 (64-bit) with the Microsoft Edge WebView2 Runtime (part of Windows 11).
- To generate: [ComfyUI](https://github.com/comfyanonymous/ComfyUI) with [Plenio](https://github.com/jplenio/Plenio-Music-Production-System)
  1.6 or newer and its YuE2 models (an older Plenio makes songs too, without stems). Projects you made keep working
  without them: playing, editing, exporting.

## Building

```powershell
pwsh tools/fetch_deps.ps1     # the SDKs and libraries at pinned versions
pwsh tools/build.ps1          # web UI + Plenio.vst3 + tests (Visual Studio 2022)
```

The bundle is `build/VST3/Release/Plenio.vst3`.

## Installing and testing

```powershell
pwsh tools/install.ps1        # into C:\Program Files\Common Files\VST3 (asks for administrator rights)
pwsh tools/install.ps1 -Uninstall
```

What to try in REAPER and Cubase, step by step: [docs/TESTING.md](docs/TESTING.md).

## Licence

Apache License 2.0 - see [LICENSE](LICENSE), [NOTICE.md](NOTICE.md) and [THIRD_PARTY.md](THIRD_PARTY.md). Songs made
with YuE2 are CC BY-NC 4.0 (non-commercial) by the model's licence.
