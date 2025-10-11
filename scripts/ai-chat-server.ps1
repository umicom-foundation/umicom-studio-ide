# -----------------------------------------------------------------------------
# Umicom Studio IDE
# PURPOSE: Project automation / helper script.
# Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
# Last updated: 2025-10-11
# -----------------------------------------------------------------------------

[CmdletBinding()]
param(
  [ValidateSet("openai","ollama")] [string]$Backend = "ollama",
  [int]$Port = 8750
)
$ErrorActionPreference = 'Stop'

Add-Type -AssemblyName System.Net.HttpListener
Import-Module "$PSScriptRoot/modules/UStudio.OpenAI.psm1" -Force -ErrorAction SilentlyContinue | Out-Null
Import-Module "$PSScriptRoot/modules/UStudio.Ollama.psm1" -Force -ErrorAction SilentlyContinue | Out-Null

$prefix = "http://localhost:$Port/"
$listener = [System.Net.HttpListener]::new()
$listener.Prefixes.Add($prefix)
$listener.Start()
Write-Host "== Sidecar on $prefix (backend: $Backend) ==" -ForegroundColor Cyan

$webDir = Join-Path (Split-Path $PSScriptRoot) "web"

while ($listener.IsListening) {
  $ctx = $listener.GetContext()
  $req = $ctx.Request
  $res = $ctx.Response
  try {
    if ($req.HttpMethod -eq "GET" -and ($req.Url.AbsolutePath -eq "/" -or $req.Url.AbsolutePath -eq "/index.html")) {
      $html = Get-Content -Raw (Join-Path $webDir "chat.html")
      $bytes = [Text.Encoding]::UTF8.GetBytes($html)
      $res.ContentType = "text/html; charset=utf-8"
      $res.OutputStream.Write($bytes, 0, $bytes.Length)
      $res.Close()
      continue
    }
    if ($req.HttpMethod -eq "GET" -and $req.Url.AbsolutePath -eq "/app.js") {
      $js = Get-Content -Raw (Join-Path $webDir "app.js")
      $bytes = [Text.Encoding]::UTF8.GetBytes($js)
      $res.ContentType = "application/javascript; charset=utf-8"
      $res.OutputStream.Write($bytes, 0, $bytes.Length)
      $res.Close()
      continue
    }
    if ($req.HttpMethod -eq "GET" -and $req.Url.AbsolutePath -eq "/styles.css") {
      $css = Get-Content -Raw (Join-Path $webDir "styles.css")
      $bytes = [Text.Encoding]::UTF8.GetBytes($css)
      $res.ContentType = "text/css; charset=utf-8"
      $res.OutputStream.Write($bytes, 0, $bytes.Length)
      $res.Close()
      continue
    }

    if ($req.HttpMethod -eq "POST" -and $req.Url.AbsolutePath -eq "/chat") {
      $reader = New-Object IO.StreamReader($req.InputStream, $req.ContentEncoding)
      $raw    = $reader.ReadToEnd()
      $body   = $raw | ConvertFrom-Json
      $prompt = [string]$body.prompt
      $model  = [string]$body.model

      if ([string]::IsNullOrWhiteSpace($prompt)) {
        $j = @{ ok=$false; error="missing prompt" } | ConvertTo-Json
        $b = [Text.Encoding]::UTF8.GetBytes($j)
        $res.StatusCode = 400
        $res.ContentType = "application/json; charset=utf-8"
        $res.OutputStream.Write($b,0,$b.Length); $res.Close(); continue
      }

      try {
        $reply = if ($Backend -eq "openai") {
          Invoke-UStudioOpenAI -Prompt $prompt -Model $model
        } else {
          Invoke-UStudioOllama -Prompt $prompt -Model $model
        }
        $j = @{ ok=$true; reply=$reply } | ConvertTo-Json -Depth 3
        $b = [Text.Encoding]::UTF8.GetBytes($j)
        $res.ContentType = "application/json; charset=utf-8"
        $res.OutputStream.Write($b, 0, $b.Length)
        $res.Close()
      } catch {
        $j = @{ ok=$false; error=$_.Exception.Message } | ConvertTo-Json
        $b = [Text.Encoding]::UTF8.GetBytes($j)
        $res.StatusCode = 500
        $res.ContentType = "application/json; charset=utf-8"
        $res.OutputStream.Write($b,0,$b.Length); $res.Close()
      }
      continue
    }

    $res.StatusCode = 404
    $res.Close()
  } catch {
    $res.StatusCode = 500
    $bytes = [Text.Encoding]::UTF8.GetBytes("{""ok"":false,""error"":""" + $_.Exception.Message.Replace('"','\"') + """}")
    $res.OutputStream.Write($bytes, 0, $bytes.Length)
    $res.Close()
  }
}
