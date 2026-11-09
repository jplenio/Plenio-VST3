# Releasing Plenio VST3

From the local repository to a published release. The first time includes Plenio 1.6.0 and the GitHub repository;
after that, a release is a version number, a tag and a click. GitHub Actions builds, tests and packs everything;
publishing stays yours.

**The first release, in this order:**

1. [The repository on GitHub](#1-the-repository-on-github-once): create it, push, settings. The YouTube videos link to
   it already.
2. [Plenio 1.6.0](#2-plenio-160-before-the-first-release): the plug-in needs it for stems and measured bars.
3. [The tag](#3-the-tag): GitHub Actions builds the release and makes a draft.
4. [The draft](#4-the-draft-try-it-then-publish): try the installer, then publish.
5. [Afterwards](#5-afterwards): YouTube's descriptions, the posts.

## 0. The tools (once)

- **Git** with Git Credential Manager (part of Git for Windows): the first push opens the browser to sign in.
- **The GitHub CLI**: `winget install --id GitHub.cli`, then in a new terminal `gh auth login` (*GitHub.com* ›
  *HTTPS* › *Login with a web browser*). The scripts below use it.
- **PowerShell 7** (`pwsh`), as for building.

## 1. The repository on GitHub (once)

In this repository's folder:

```powershell
pwsh tools/release/setup_github.ps1 -DryRun    # checks everything, changes nothing
pwsh tools/release/setup_github.ps1
```

It checks that `gh` is signed in as `jplenio` and that `main` has nothing uncommitted, then:

1. creates **`jplenio/Plenio-VST3`** (public) with its description and website;
2. adds the remote `origin` and pushes `main`: **CI** starts (tab *Actions*);
3. sets the topics (from [launch/ANNOUNCEMENTS.md › GitHub](launch/ANNOUNCEMENTS.md#github)), turns Discussions on and
   the wiki off (the docs are in the repository), and deletes merged branches;
4. lets workflows propose pull requests (the weekly **Plenio** check does) with a read-only token by default (each
   workflow asks for what it needs);
5. makes the labels the issue forms use (`daw-report`, `idea`) and turns on private vulnerability reporting.

**By hand** (GitHub has no API for it): *Settings* › *General* › *Social preview* › *Edit* › *Upload an image* ›
`docs/media/social-preview.png`.

Running it again is safe: what is there stays.

<details>
<summary>Without the GitHub CLI: the same in the browser</summary>

1. [github.com/new](https://github.com/new): owner `jplenio`, name **`Plenio-VST3`**, *Public*, **no** README,
   `.gitignore` or licence (the repository has them) › *Create repository*.
2. Push:
   ```powershell
   git remote add origin https://github.com/jplenio/Plenio-VST3.git
   git push -u origin main
   ```
3. The repository's page › *About* (the gear): the description and website from
   [launch/ANNOUNCEMENTS.md › GitHub](launch/ANNOUNCEMENTS.md#github), and the topics, each followed by a space so it
   turns into a chip, then *Save changes*.
4. *Settings* › *General*: *Social preview* as above; under *Features* Discussions on, Wikis off; under *Pull Requests*
   *Automatically delete head branches*.
5. *Settings* › *Actions* › *General* › *Workflow permissions*: **Allow GitHub Actions to create and approve pull
   requests** › *Save*.
6. *Issues* › *Labels* › *New label*: `daw-report` and `idea`.

</details>

**The first CI run** (tab *Actions*, about 15 minutes): Windows builds the plug-in, runs the UI tests, the unit tests
and the VST3 validator and packs a zip (under the run: *Artifacts*); Linux builds the core and its tests; macOS builds
the bundle and its package. macOS is experimental: if it fails, the run still counts. A red Windows or Linux job: its
log says what to fix.

**Dependabot** proposes newer versions of the Actions as pull requests (monthly): merge them when their CI is green.

## 2. Plenio 1.6.0 (before the first release)

Plenio VST3 recommends Plenio 1.6 (stems with every take, the bars measured in a take; with 1.5 it makes songs
without them). Its release commit is ready on the branch `song-api-1.4` of the Plenio repository: version 1.6.0, the
changelog dated, the frontend rebuilt, a README section *Plenio in your DAW*. Checked on this machine: ruff, mypy, the
unit tests, the frontend's tests, the generators without a diff, `comfy node validate` and `comfy node pack`.
Plenio's own checklist is its [docs/dev/release.md](https://github.com/jplenio/Plenio-Music-Production-System/blob/main/docs/dev/release.md).

In the Plenio repository's folder (next to this one):

```powershell
git push origin song-api-1.4     # Plenio's CI runs on the branch (Python, ComfyUI, frontend, package): wait for green
git switch main
git merge --ff-only song-api-1.4
git push origin main
gh release create v1.6.0 --target main --title "Plenio Music Production System 1.6.0" `
  --notes-file ..\plenio-vst3-launch\plenio-1.6.0-release.md
```

The release's text is ready in `../plenio-vst3-launch/plenio-1.6.0-release.md` (like 1.5.1's: what Plenio is, the
videos, then what is new). Publishing the release starts Plenio's **Publish** workflow: it attaches the node's zip, the
six templates and `SHA256SUMS.txt`, and puts 1.6.0 into the Comfy Registry (`REGISTRY_ACCESS_TOKEN`, set since the
earlier releases).

Then:

- *Actions*: **Publish to Comfy Registry** green; the release has its eight files.
- The [Registry's page](https://registry.comfy.org/publishers/jplenio/nodes/comfyui-plenio-music) shows 1.6.0.
- Your own ComfyUI: update Plenio (ComfyUI Manager › *Update*, or `git pull` in `custom_nodes`), restart it. Plenio
  VST3's status shows 1.6.0 and no hint about an older Plenio.
- Plenio's real-model smoke tests, if you like, on your machine ([its testing.md](https://github.com/jplenio/Plenio-Music-Production-System/blob/main/docs/dev/testing.md)).

The plug-in has 1.6.0 as tested already (`resources/plenio-compat.json`): the weekly **Plenio** workflow will find
nothing to propose.

## 3. The tag

Back in this repository's folder:

```powershell
git pull                                     # whatever was merged on GitHub meanwhile (Dependabot)
pwsh tools/release/tag_release.ps1 -DryRun   # checks, makes no tag
pwsh tools/release/tag_release.ps1
```

It checks the version (`CMakeLists.txt`), its section in `CHANGELOG.md` (dated **today**: change the date, commit and
push if the release is on another day, or pass `-AnyDate`), the release's text `docs/release/v<version>.md`, that
`main` has nothing uncommitted and GitHub has the same commit, and that the tag is new; then it tags
`v0.1.0-alpha` and pushes the tag.

The tag starts **Release** (tab *Actions*, about 25 minutes): it builds and tests again, makes the Windows installer
and zip and the macOS package, and creates a **draft** under *Releases*, titled *Plenio VST3 0.1.0-alpha*, marked
*pre-release*, with the text of `docs/release/v0.1.0-alpha.md` and the files with their `SHA256SUMS.txt`.

## 4. The draft: try it, then publish

1. *Releases* › the draft: `Plenio-VST3-0.1.0-alpha-windows-x64-setup.exe`, `…-windows-x64.zip`,
   `…-macos-universal.pkg` (missing if the experimental macOS build failed) and `SHA256SUMS.txt`.
2. Download the installer and install it (SmartScreen: *More info* › *Run anyway*; the installers are not signed yet).
   In REAPER: Plenio on a track, a saved song opens and plays, *Generate* makes a take with stems.
3. *Edit* (the pencil): drag `../plenio-vst3-launch/web/plenio-vst3-teaser-720p.mp4` onto the empty line under the
   video links (GitHub uploads it and plays it in place), *Set as a pre-release* stays ticked, then
   **Publish release**.
4. The README's *release* badge shows `v0.1.0-alpha`; the download links work.

## 5. Afterwards

- **YouTube**: the final descriptions of both videos, with the tutorial's chapters, are in
  [launch/ANNOUNCEMENTS.md › YouTube](launch/ANNOUNCEMENTS.md#youtube): paste them in YouTube Studio.
- **The posts**: [launch/README.md](launch/README.md), the order and which picture goes where.
- New issues: the form *How it works in my DAW* (label `daw-report`) says which DAWs work.

## When something goes wrong

- **CI is red**: the run › the red job › its log. Fix, commit, push: CI runs again. A red macOS job does not block.
- **The Release workflow failed**: a flaky download: the run › *Re-run failed jobs*. A fix in the code: delete the tag
  (`git push origin :refs/tags/v0.1.0-alpha`, then `git tag -d v0.1.0-alpha`) and a draft if one was made, fix,
  commit, push, then `tag_release.ps1` again.
- **A new draft for the same tag** (after the macOS job was fixed, say): delete the draft, then *Actions* › *Release* ›
  *Run workflow* with the tag.
- **`git push` refused with `GH007`**: a commit carries an e-mail address GitHub keeps private
  (*Settings* › *Emails* › *Block command line pushes that expose my email*). Allow it there, or rewrite the commit's
  author before the first push.

## Later versions

1. The version is in `CMakeLists.txt`: `project(PlenioVST3 VERSION 0.1.1 …)` and `PLENIO_VERSION_SUFFIX` (`alpha`,
   `beta`, or empty for a final version); `webui/package.json` keeps the same number.
2. `CHANGELOG.md` gets the version's section and its link at the end; `docs/release/v<version>.md` the release's text
   (copy the last one).
3. Commit, push, then [3. The tag](#3-the-tag) and [4. The draft](#4-the-draft-try-it-then-publish).

The **Plenio** workflow checks every Monday whether the newest Plenio release still has everything the plug-in uses
(`tools/plenio.py`). Compatible: it takes over the new styles and records the version as tested, as a pull request:
merge it, and the next release lists that Plenio as tested. Not compatible: it opens an issue with the report.

## Code signing (later)

The installers are not signed yet: Windows SmartScreen shows *unknown publisher* (*More info* › *Run anyway*), and
macOS needs a right-click › *Open* on the package. To sign:

- **Windows**: a code-signing certificate (or Azure Trusted Signing); a `signtool sign` step in `release.yml` before
  `tools/package.ps1` packs (for `Plenio.vst3`) and after it (for the installer), the certificate as a repository secret.
- **macOS**: an Apple Developer ID ($99 a year); `codesign` with it instead of the ad-hoc signature in
  `installer/macos/build_pkg.sh`, `productsign` for the package and `xcrun notarytool submit --wait`, the credentials as
  secrets.
