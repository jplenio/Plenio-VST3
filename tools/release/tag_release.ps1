#Requires -Version 7
<#
.SYNOPSIS
    Tags a Plenio VST3 release and pushes the tag; GitHub Actions then builds, tests and drafts the release.

.DESCRIPTION
    Checks first, and stops at the first thing that is not ready:
      - the version: CMakeLists.txt (project VERSION and PLENIO_VERSION_SUFFIX), the tag v<version> not used yet
      - CHANGELOG.md has the version's section, dated today (-AnyDate allows another date), and its link at the end
      - docs/release/v<version>.md, the release's text
      - on main, nothing uncommitted, and GitHub has this very commit (git push first)
    Then: git tag -a v<version>, git push origin v<version>. -DryRun only checks.

    The draft appears under Releases about 25 minutes later (.github/workflows/release.yml); publishing it stays yours.

.EXAMPLE
    pwsh tools/release/tag_release.ps1 -DryRun
    pwsh tools/release/tag_release.ps1
#>
param([switch]$AnyDate, [switch]$DryRun)
$ErrorActionPreference = 'Stop'
Set-Location (Resolve-Path (Join-Path $PSScriptRoot '..\..'))

function Say([string]$Text) { Write-Host "  ok  $Text" }
function Probe([scriptblock]$Command) {
    $saved = $ErrorActionPreference; $ErrorActionPreference = 'Continue'
    try { $out = & $Command 2>$null; return [pscustomobject]@{ Ok = ($LASTEXITCODE -eq 0); Out = ($out -join "`n").Trim() } }
    finally { $ErrorActionPreference = $saved }
}

# The version
$cmake = Get-Content CMakeLists.txt -Raw
$m = [regex]::Match($cmake, 'project\(PlenioVST3\s+VERSION\s+(\d+\.\d+\.\d+)')
if (-not $m.Success) { throw 'No project(PlenioVST3 VERSION x.y.z) in CMakeLists.txt' }
$version = $m.Groups[1].Value
$suffix = [regex]::Match($cmake, 'set\(PLENIO_VERSION_SUFFIX\s+"([^"]*)"\)').Groups[1].Value
if ($suffix) { $version = "$version-$suffix" }
$tag = "v$version"
Write-Host "Plenio VST3 $version (tag $tag)" -ForegroundColor Cyan
$ui = (Get-Content webui/package.json -Raw | ConvertFrom-Json).version
if ($ui -ne $m.Groups[1].Value) { Write-Warning "webui/package.json says $ui, CMakeLists.txt $($m.Groups[1].Value) (the UI's version is not shown anywhere; fix it when convenient)" }

# The changelog and the release's text
$changelog = Get-Content CHANGELOG.md -Raw
$section = [regex]::Match($changelog, "(?m)^## \[$([regex]::Escape($version))\] - (\d{4}-\d{2}-\d{2})")
if (-not $section.Success) { throw "CHANGELOG.md has no section '## [$version] - <date>'" }
$date = $section.Groups[1].Value
$today = Get-Date -Format 'yyyy-MM-dd'
if ($date -ne $today -and -not $AnyDate) { throw "CHANGELOG.md dates $version $date, today is ${today}: change the date (and commit, push), or run with -AnyDate" }
Say "CHANGELOG.md: $version, $date"
if ($changelog -notmatch "(?m)^\[$([regex]::Escape($version))\]: https://github\.com/jplenio/Plenio-VST3/releases/tag/$([regex]::Escape($tag))") {
    throw "CHANGELOG.md lacks the link at its end: [$version]: https://github.com/jplenio/Plenio-VST3/releases/tag/$tag"
}
$notes = "docs/release/$tag.md"
if (-not (Test-Path $notes)) { throw "$notes is missing: the release's text (copy the last one and change it)" }
Say "$notes"

# Git: main, clean, on GitHub, the tag still free
$branch = (Probe { git rev-parse --abbrev-ref HEAD }).Out
if ($branch -ne 'main') { throw "On branch '$branch': releases are tagged on main" }
$dirty = (Probe { git status --porcelain }).Out
if ($dirty) { throw "Uncommitted changes:`n$dirty" }
if (-not (Probe { git fetch origin main --quiet }).Ok) { throw 'git fetch origin main failed: is the remote origin there (tools/release/setup_github.ps1)?' }
$head = (Probe { git rev-parse HEAD }).Out
$remote = (Probe { git rev-parse origin/main }).Out
if (-not $remote) { throw 'GitHub has no main yet: tools/release/setup_github.ps1 pushes it' }
if ($head -ne $remote) { throw "GitHub's main is $($remote.Substring(0, 7)), yours $($head.Substring(0, 7)): git push (or git pull) first" }
Say "main $($head.Substring(0, 7)), the same on GitHub"
if ((Probe { git rev-parse -q --verify "refs/tags/$tag" }).Ok) { throw "The tag $tag exists here already (git tag -d $tag, if it is a leftover)" }
if ((Probe { git ls-remote --tags origin "refs/tags/$tag" }).Out) { throw "GitHub has the tag $tag already" }
Say "the tag $tag is free"

if ($DryRun) { Write-Host "`nReady. (-DryRun: no tag made)" -ForegroundColor Yellow; return }
git tag -a $tag -m "Plenio VST3 $version"
if ($LASTEXITCODE -ne 0) { throw 'git tag failed' }
git push origin $tag
if ($LASTEXITCODE -ne 0) { throw "git push of the tag failed (the tag stays here: git push origin $tag, or git tag -d $tag)" }
Write-Host "`nTagged and pushed. The release workflow runs now (about 25 minutes):" -ForegroundColor Green
Write-Host '  https://github.com/jplenio/Plenio-VST3/actions/workflows/release.yml'
Write-Host '  then the draft: https://github.com/jplenio/Plenio-VST3/releases'
