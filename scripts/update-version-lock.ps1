#-----------------------------------------------------------------------------
# Umicom Studio IDE
# File: scripts/update-version-lock.ps1
#
# PURPOSE:
#   Record the exact Studio, Framework, AuthorEngine and llama.cpp commits used
#   by the current checkout.  The script changes only VERSION_LOCK.json.
#
# Created by: Sammy Hegab
# Organisation: Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------
[CmdletBinding()]
param(
    [string]$RepositoryRoot = (Get-Location).Path
)

$ErrorActionPreference = "Stop"

function Get-GitCommit {
    param([Parameter(Mandatory = $true)][string]$WorkingDirectory)

    $commit = git -C $WorkingDirectory rev-parse HEAD
    if ($LASTEXITCODE -ne 0) {
        throw "Could not read the Git commit for: $WorkingDirectory"
    }
    return $commit.Trim()
}

function Get-CMakeVersion {
    param(
        [Parameter(Mandatory = $true)][string]$Path,
        [Parameter(Mandatory = $true)][string]$Pattern
    )

    $content = Get-Content -LiteralPath $Path -Raw
    $match = [regex]::Match(
        $content,
        $Pattern,
        [System.Text.RegularExpressions.RegexOptions]::Singleline)
    if (-not $match.Success) {
        throw "Could not read the CMake version from: $Path"
    }
    return $match.Groups[1].Value
}

$studioRoot = (Resolve-Path $RepositoryRoot).Path
$frameworkRoot = Join-Path $studioRoot "framework"
$authorEngineRoot = Join-Path $studioRoot "third_party\umicom\uengine"
$llamaRoot = Join-Path $authorEngineRoot "third_party\llama.cpp"
$studioVersion = Get-CMakeVersion `
    -Path (Join-Path $studioRoot "applications\studio\CMakeLists.txt") `
    -Pattern 'set\s*\(\s*UMICOM_STUDIO_VERSION\s+"([0-9]+\.[0-9]+\.[0-9]+)"'
$frameworkVersion = Get-CMakeVersion `
    -Path (Join-Path $frameworkRoot "CMakeLists.txt") `
    -Pattern 'project\s*\(\s*UmicomFramework\s+VERSION\s+([0-9]+\.[0-9]+\.[0-9]+)'

$lock = [ordered]@{
    schema = "umicom.version-lock.v1"
    studio_version = $studioVersion
    studio_parent_base_commit = Get-GitCommit -WorkingDirectory $studioRoot
    framework_version = $frameworkVersion
    framework_commit = Get-GitCommit -WorkingDirectory $frameworkRoot
    authorengine_commit = Get-GitCommit -WorkingDirectory $authorEngineRoot
    llama_cpp_commit = Get-GitCommit -WorkingDirectory $llamaRoot
}

$destination = Join-Path $studioRoot "VERSION_LOCK.json"
$lock | ConvertTo-Json -Depth 4 |
    Set-Content -LiteralPath $destination -Encoding utf8
Write-Host "Updated $destination" -ForegroundColor Green
