#Requires -Version 7
<#
.SYNOPSIS
    Puts Plenio VST3 on GitHub, once: the repository, the first push, its description, topics, labels and settings.

.DESCRIPTION
    Needs the GitHub CLI, signed in:  winget install --id GitHub.cli  then  gh auth login  (once).
    Safe to run again: what is there already stays. -DryRun only checks and shows what it would do.

    What it does, in this order:
      1. checks: gh signed in as the repository's owner, on branch main, nothing uncommitted
      2. creates the repository on GitHub (public, with the description and website), if it is not there yet
      3. adds the remote origin and pushes main (CI starts: tab Actions)
      4. topics, Discussions on, the wiki off (the docs are in the repository), branches deleted after a merge
      5. Actions may propose pull requests (the weekly Plenio check does); their token is read-only unless a
         workflow asks for more (all three do, for what they need)
      6. the labels the issue forms use (daw-report, idea), private vulnerability reporting
    By hand afterwards (GitHub has no API for it): Settings > General > Social preview > docs/media/social-preview.png

.EXAMPLE
    pwsh tools/release/setup_github.ps1 -DryRun
    pwsh tools/release/setup_github.ps1
#>
param(
    [string]$Repo = 'jplenio/Plenio-VST3',
    [ValidateSet('public', 'private')][string]$Visibility = 'public',
    [switch]$NoDiscussions,
    [switch]$DryRun
)
$ErrorActionPreference = 'Stop'
Set-Location (Resolve-Path (Join-Path $PSScriptRoot '..\..'))

$owner = $Repo.Split('/')[0]
$description = 'The Plenio Music Production System in your DAW: songs, takes, stems, lyrics, melody and chords, made on your own GPU. VST3 + ARA 2.'
$homepage = "https://github.com/$Repo#readme"
$topics = @('vst3', 'vst3-plugin', 'ara2', 'audio-plugin', 'daw', 'reaper', 'music-production', 'ai-music', 'songwriting',
    'comfyui', 'yue', 'generative-ai', 'local-ai', 'midi', 'stems', 'cpp20', 'vue')
$labels = @(
    @{ Name = 'daw-report'; Color = '8a4dff'; Description = 'How Plenio VST3 works in a DAW' },
    @{ Name = 'idea'; Color = 'ffb23e'; Description = 'Something Plenio VST3 could do, or do differently' }
)

function Say([string]$Text) { Write-Host "  $Text" }
function Step([string]$Text) { Write-Host "`n== $Text" -ForegroundColor Cyan }

# A native command's output and whether it succeeded, without its error text (gh says "not found" on stderr).
function Probe([scriptblock]$Command) {
    $saved = $ErrorActionPreference; $ErrorActionPreference = 'Continue'
    try { $out = & $Command 2>$null; return [pscustomobject]@{ Ok = ($LASTEXITCODE -eq 0); Out = ($out -join "`n").Trim() } }
    finally { $ErrorActionPreference = $saved }
}

# Runs a native command that changes something, or only shows it with -DryRun; stops on failure.
function Change([string]$What, [scriptblock]$Command) {
    if ($DryRun) { Say "would: $What"; return }
    Say $What
    & $Command
    if ($LASTEXITCODE -ne 0) { throw "Failed: $What (exit code $LASTEXITCODE)" }
}

Step 'Checks'
if (-not (Get-Command gh -ErrorAction SilentlyContinue)) {
    throw 'The GitHub CLI is missing: winget install --id GitHub.cli   (then open a new terminal and run: gh auth login)'
}
$login = Probe { gh api user --jq .login }
if (-not $login.Ok) { throw 'The GitHub CLI is not signed in: gh auth login   (GitHub.com, HTTPS, sign in with a web browser)' }
if ($login.Out -ne $owner) { throw "gh is signed in as '$($login.Out)', the repository belongs to '$owner': gh auth login as $owner" }
Say "gh: signed in as $($login.Out)"
$branch = (Probe { git rev-parse --abbrev-ref HEAD }).Out
if ($branch -ne 'main') { throw "On branch '$branch': switch to main first (git switch main)" }
$dirty = (Probe { git status --porcelain }).Out
if ($dirty) { throw "Uncommitted changes:`n$dirty`nCommit them (or stash them) first." }
Say "git: on main, nothing uncommitted, $((Probe { git rev-list --count HEAD }).Out) commits"

Step "The repository $Repo"
if ((Probe { gh repo view $Repo --json name }).Ok) {
    Say 'is there already'
} else {
    Change "create it ($Visibility)" { gh repo create $Repo "--$Visibility" --description $description --homepage $homepage }
}

Step 'Push main'
$url = "https://github.com/$Repo.git"
$origin = Probe { git remote get-url origin }
if (-not $origin.Ok) {
    Change "add the remote origin: $url" { git remote add origin $url }
} elseif ($origin.Out -notmatch [regex]::Escape("github.com/$Repo")) {
    throw "The remote origin points elsewhere: $($origin.Out)"
}
Change 'git push -u origin main   (the first time, Git may open the browser to sign in)' { git push -u origin main }

Step 'Description, topics, features'
$edit = @('repo', 'edit', $Repo, '--description', $description, '--homepage', $homepage, '--add-topic', ($topics -join ','),
    '--enable-wiki=false', '--delete-branch-on-merge')
if (-not $NoDiscussions) { $edit += '--enable-discussions' }
Change "$($topics.Count) topics, the wiki off, Discussions $(if ($NoDiscussions) { 'as they are' } else { 'on' })" { gh @edit }

Step 'GitHub Actions'
Change 'workflows may propose pull requests; their token reads unless a workflow asks for more' {
    gh api --method PUT "repos/$Repo/actions/permissions/workflow" -f default_workflow_permissions=read -F can_approve_pull_request_reviews=true
}

Step 'Labels and security'
foreach ($l in $labels) {
    Change "label $($l.Name)" { gh label create $l.Name --repo $Repo --color $l.Color --description $l.Description --force }
}
Change 'private vulnerability reporting on' { gh api --method PUT "repos/$Repo/private-vulnerability-reporting" --silent }

Step 'Done. By hand:'
Say "1. The social preview: https://github.com/$Repo/settings  > Social preview > Edit > Upload an image > docs/media/social-preview.png"
Say "2. The first CI run:    https://github.com/$Repo/actions   (Windows about 15 min; macOS is experimental and may fail)"
Say "   or here:             gh run watch --repo $Repo"
if ($DryRun) { Write-Host "`n(-DryRun: nothing was changed)" -ForegroundColor Yellow }
