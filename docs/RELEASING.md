# Releasing Plenio VST3

From the local repository to a published release on GitHub. The first time includes setting up the GitHub repository;
after that, a release is a tag and a click. GitHub Actions builds, tests and packages everything; publishing stays
yours.

## 0. Before the first release: Plenio 1.6.0

Plenio VST3 recommends Plenio 1.6 (stems with every take, the bars measured in a take). 1.6.0 lives on the branch
`song-api-1.4` of the Plenio repository and is not released yet: release it first, the way Plenio releases
(its `docs/dev/release.md`), so that people who install Plenio VST3 can get it.

```powershell
cd D:\Daten2\Deepseek\ComfyUI-MiniMax\Plenio-Music-Production-System
git switch main
git merge --ff-only song-api-1.4      # or a pull request on GitHub
git tag -a v1.6.0 -m "Plenio 1.6.0"
git push origin main v1.6.0
```

## 1. Create the GitHub repository (once)

**In the browser:** [github.com/new](https://github.com/new) › owner `jplenio`, name **`Plenio-VST3`**, *Public*, and
**no** README, `.gitignore` or licence (the repository has them) › *Create repository*.

**Or with the GitHub CLI** (`winget install GitHub.cli`, then `gh auth login` once):

```powershell
cd D:\Daten2\Deepseek\ComfyUI-MiniMax\Plenio-VST3
gh repo create jplenio/Plenio-VST3 --public --source . --remote origin `
  --description "The Plenio Music Production System in your DAW: songs, takes, stems, lyrics, melody and chords, made on your own GPU. VST3 + ARA 2." `
  --homepage "https://github.com/jplenio/Plenio-VST3#readme"
```

## 2. Push

```powershell
cd D:\Daten2\Deepseek\ComfyUI-MiniMax\Plenio-VST3
git remote add origin https://github.com/jplenio/Plenio-VST3.git   # not needed after gh repo create
git branch -M main
git push -u origin main
```

## 3. Settings (once)

On the repository's page:

1. **About** (the gear next to it): the description from above, the website `https://github.com/jplenio/Plenio-VST3#readme`,
   and the topics from [launch/ANNOUNCEMENTS.md › GitHub](launch/ANNOUNCEMENTS.md#github). Each topic has to turn into a
   chip before *Save changes*: type them one by one, each followed by a space.
2. **Settings › General › Social preview** › *Edit* › upload `docs/media/social-preview.png`.
3. **Settings › General › Features**: *Discussions* on, if you want questions there rather than in issues.
4. **Settings › Actions › General › Workflow permissions**: *Read and write permissions* and **Allow GitHub Actions to
   create and approve pull requests** (the Plenio check proposes its results as pull requests).

## 4. The first CI run

The push started **CI** (tab *Actions*): Windows builds the plug-in, runs the tests and the VST3 validator, and packs a
zip (under the run: *Artifacts*); Linux builds the core and its tests; macOS builds the bundle and its package. macOS is
marked experimental: if it fails, the rest still counts. Its log tells what to fix (send it over).

## 5. Release

```powershell
git tag -a v0.1.0-alpha -m "Plenio VST3 0.1.0-alpha"
git push origin v0.1.0-alpha
```

The tag starts **Release** (tab *Actions*, about 25 minutes): it builds and tests again, makes the Windows installer
(Inno Setup comes with GitHub's Windows machines), the zips, the macOS package and `SHA256SUMS.txt`, and creates a
**draft** under *Releases*, titled *Plenio VST3 0.1.0-alpha*, with the text of `docs/release/v0.1.0-alpha.md` and
*pre-release* ticked.

## 6. Look at the draft, then publish

1. Download the installer from the draft, install it, open REAPER: Plenio is there, a song opens and plays.
2. **The videos** (they are not in the repository: their music is a YuE2 song, CC BY-NC 4.0): upload the teaser and the
   tutorial to YouTube ([launch/README.md](launch/README.md) has the files, [ANNOUNCEMENTS.md](launch/ANNOUNCEMENTS.md#youtube)
   their titles and descriptions). Then, in the draft (*Edit*), replace the line *▶ Teaser and tutorial* with their links,
   and drag the 720p teaser onto an empty line after the first paragraph: GitHub plays videos up to 10 MB in place.
3. **Publish release.**

Then put the YouTube links into the README too (the comment under the picture), commit and push.

## 7. Tell people

[launch/README.md](launch/README.md): the order, the texts for every channel, and which picture or video goes with which
post.

## Later versions

1. The version is in `CMakeLists.txt`: `project(PlenioVST3 VERSION 0.1.1 …)` and `PLENIO_VERSION_SUFFIX` (`alpha`,
   `beta`, or empty for a final version).
2. `CHANGELOG.md` gets the version's section; `docs/release/v<version>.md` the release's text (the workflow takes it; without
   it, the changelog).
3. Commit, push, tag `v<version>`, push the tag: back to step 5.

The **Plenio** workflow checks every Monday whether a new Plenio still has everything the plug-in uses, and proposes a
pull request (or opens an issue): merge it, and the next release lists the new Plenio as tested.

## Code signing (later)

The installers are not signed yet: Windows SmartScreen shows *unknown publisher* (*More info › Run anyway*), and macOS
needs a right-click › *Open* on the package. To sign:

- **Windows**: a code-signing certificate (or Azure Trusted Signing); a `signtool sign` step in `release.yml` before
  `tools/package.ps1` packs (for `Plenio.vst3`) and after it (for the installer), the certificate as a repository secret.
- **macOS**: an Apple Developer ID ($99 a year); `codesign` with it instead of the ad-hoc signature in
  `installer/macos/build_pkg.sh`, `productsign` for the package and `xcrun notarytool submit --wait`, the credentials as
  secrets.
