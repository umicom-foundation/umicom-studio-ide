#-----------------------------------------------------------------------------
# Umicom Studio IDE
# File: scripts/update-version-lock.ps1
#
# PURPOSE:
#   Record the exact Framework, AuthorEngine and llama.cpp commits consumed by
#   the current Studio checkout. The script is read-only apart from writing
#   VERSION_LOCK.json at the Studio repository root.
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

$studioRoot = (Resolve-Path $RepositoryRoot).Path
$frameworkRoot = Join-Path $studioRoot "framework"
$authorEngineRoot = Join-Path $studioRoot "third_party\umicom\uengine"
$llamaRoot = Join-Path $authorEngineRoot "third_party\llama.cpp"

$lock = [ordered]@{
    schema = "umicom.version-lock.v1"
    studio_version = "0.11.1"
    studio_parent_base_commit = Get-GitCommit -WorkingDirectory $studioRoot
    framework_version = "0.4.2"
    framework_commit = Get-GitCommit -WorkingDirectory $frameworkRoot
    authorengine_commit = Get-GitCommit -WorkingDirectory $authorEngineRoot
    llama_cpp_commit = Get-GitCommit -WorkingDirectory $llamaRoot
}

$destination = Join-Path $studioRoot "VERSION_LOCK.json"
$lock | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath $destination -Encoding utf8
Write-Host "Updated $destination" -ForegroundColor Green
