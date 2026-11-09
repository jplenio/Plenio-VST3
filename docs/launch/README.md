# Launch plan for 0.1.0-alpha

What is ready, where it is, and the order to publish it in. Nothing here publishes by itself.

## Ready in the repository

| What | Where |
|---|---|
| Version `0.1.0-alpha`, the changelog | `CMakeLists.txt`, [`CHANGELOG.md`](../../CHANGELOG.md) |
| Licences and notices | [`LICENSE`](../../LICENSE), [`NOTICE.md`](../../NOTICE.md), [`THIRD_PARTY.md`](../../THIRD_PARTY.md) |
| README, tutorial, the DAWs, development, releasing | [`README.md`](../../README.md), [`TUTORIAL.md`](../TUTORIAL.md), [`DAWS.md`](../DAWS.md), [`DEVELOPMENT.md`](../DEVELOPMENT.md), [`RELEASING.md`](../RELEASING.md) |
| The release's text (the release workflow takes it) | [`docs/release/v0.1.0-alpha.md`](../release/v0.1.0-alpha.md) |
| The texts for every channel, and which picture goes where | [`ANNOUNCEMENTS.md`](ANNOUNCEMENTS.md) |
| Logo, banner, social preview, screenshots, animations | [`docs/media/`](../media/) |
| The installers and the release itself | made by GitHub Actions from the tag (`.github/workflows/release.yml`) |
| The repository's setup and the tag, with their checks | `tools/release/setup_github.ps1`, `tools/release/tag_release.ps1` ([`RELEASING.md`](../RELEASING.md)) |
| The issue forms: a bug, an idea, *How it works in my DAW* | `.github/ISSUE_TEMPLATE/` |

## The files

Outside the repository, in `../plenio-vst3-launch/` (next to this repository; not committed: the videos' music is a
YuE2 song, CC BY-NC 4.0, which the repository's Apache-2.0 does not cover):

| File | What |
|---|---|
| `video/plenio-vst3-teaser.mp4` | the teaser, 1920 x 1080, 72 s, the music at -14 LUFS |
| `video/plenio-vst3-teaser-vertical.mp4` | the teaser for Shorts, Reels and TikTok, 1080 x 1920 |
| `video/plenio-vst3-tutorial.mp4` | the tutorial, 1920 x 1080, 2:51, with chapters (`tutorial-chapters.txt`, for YouTube's description: the first at 0:00, each 10 s or longer, else YouTube shows none) |
| `video/teaser-poster.png`, `video/tutorial-thumbnail.png` | pictures for YouTube and the posts |
| `web/*-720p.mp4` | 720p copies under 10 MB: for Discord and for a player in GitHub's release text |
| `SHA256SUMS.txt` | the checksums |
| `plenio-1.6.0-release.md` | the text of Plenio 1.6.0's GitHub release ([RELEASING.md › 2](../RELEASING.md#2-plenio-160-before-the-first-release)) |

Make them again (after a new demo song or new screenshots):

```powershell
node tools/media/tour.mjs .dev/tour new-song takes lyrics notes daw cover library   # the plug-in in EditorHost or a DAW
python tools/media/make_media.py anims .dev/tour                                     # docs/media/anim/*.gif
python tools/media/make_teaser.py "<song folder>" .dev/tour ../plenio-vst3-launch/video
python tools/media/make_tutorial.py "<song folder>" .dev/tour ../plenio-vst3-launch/video/plenio-vst3-tutorial.mp4
```

The 720p copies: `ffmpeg -i <video> -vf scale=-2:720 -c:v libx264 -b:v 900k -pass 1/2 … -c:a aac -b:a 128k -movflags +faststart`
(the vertical one scaled to 720 x 1280).

**REAPER's pictures and clips** (`tools/media/reaper/`, REAPER 7 on Windows, the screen at 2560 x 1440):

1. Back up `%APPDATA%\REAPER\REAPER.ini` (it goes back after the session: REAPER writes window places and recent files
   into it).
2. Start REAPER with `PLENIO_VST3_HOME=<repo>\.dev\demo-home` (the demo songs, not your own) and
   `WEBVIEW2_ADDITIONAL_BROWSER_ARGUMENTS=--remote-debugging-port=9334` (so `tour.mjs` and `cdp.mjs` reach Plenio's
   window with `PLENIO_CDP_PORT=9334`); `window.ps1 -Title 'REAPER v7' -X 0 -Y 0 -Width 2560 -Height 1440`.
3. The scripts run in the open REAPER: `reaper.exe -nonewinst tools\media\reaper\<script>.lua`. `insert_dialog.lua`
   opens *Insert virtual instrument on new track*; type `plenio` (`window.ps1 -Keys`), double-click Plenio
   (`mouse.ps1`), answer the routing question with *No*. `plenio_float.lua` shows Plenio in a window of its own,
   `plenio_hide.lua` / `plenio_show.lua` hide and show it, `zoom_project.lua`, `play.lua`, `stop.lua`; `save_demo.lua`
   saves the demo project into `.dev/reaper-demo/`, `quit.lua` quits.
4. Pictures: `capture.ps1 -Screen -Out <png>`. Clips: `ffmpeg -f gdigrab -framerate 30 -offset_x 0 -offset_y 0
   -video_size 2560x1440 -draw_mouse 1 -t <s> -i desktop …` while the steps run, cut and scaled to 1920 x 1080 as
   `.dev/tour/reaper-insert.mp4` and `.dev/tour/reaper-tracks.mp4` (the tutorial takes them from there). *Put on tracks*
   with the drawn pointer: `node tools/media/tour.mjs .dev/tour reaper-put`.
5. The animation for the tutorial: `ffmpeg -i .dev/tour/reaper-tracks.mp4 -vf "fps=12,scale=1100:-1:flags=lanczos,split[a][b];[a]palettegen=max_colors=192:stats_mode=diff[p];[b][p]paletteuse=dither=bayer:bayer_scale=4:diff_mode=rectangle" docs/media/anim/reaper-tracks.gif`.
6. Quit REAPER, put `REAPER.ini` back.

## Order

On YouTube since 2026-11-09, in the playlist [Plenio VST3](https://www.youtube.com/playlist?list=PLI7BuX18HQwM): the
[teaser](https://youtu.be/ppJcVOqIaJg) and the [tutorial](https://youtu.be/rhNspYujXto). Their descriptions link the
repository, so it comes first:

1. **The repository**: create it, push, settings ([RELEASING.md › 1](../RELEASING.md#1-the-repository-on-github-once)).
2. **Plenio 1.6.0** ([RELEASING.md › 2](../RELEASING.md#2-plenio-160-before-the-first-release)): the plug-in
   recommends it, and its README now points to Plenio VST3.
3. **The release**: tag, draft, try, publish ([RELEASING.md › 3-4](../RELEASING.md#3-the-tag)), with
   `web/plenio-vst3-teaser-720p.mp4` dragged into the release's text for a player in place.
4. **YouTube's descriptions**: paste the final ones from [ANNOUNCEMENTS.md › YouTube](ANNOUNCEMENTS.md#youtube): the
   tutorial's chapters and the download link come in with them.
5. **Posts**, one channel a day, starting on a weekday morning US time (they reward being there for the comments):
   r/comfyui, then r/Reaper, r/LocalLLaMA, r/AudioPlugins; LinkedIn, Bluesky, Mastodon, Discord and KVR in between;
   Hacker News on a weekday morning; the vertical teaser on Shorts, Instagram and TikTok. The texts:
   [ANNOUNCEMENTS.md](ANNOUNCEMENTS.md).
6. **Afterwards**: the first issues (*How it works in my DAW*) tell which DAWs work; then the DAW subreddits, with that
   DAW's screenshot.

## Licence of the launch material

The texts, logo, banner, screenshots and animations: Apache-2.0, like the repository (the name Plenio and its logo
are not licensed as marks). The videos: their pictures are the project's, their music is a YuE2 song, so each video as a
whole is **CC BY-NC 4.0**: fine for announcing a free project, not for paid advertising.
