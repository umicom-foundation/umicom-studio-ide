
function Invoke-UStudioOllama {
  [CmdletBinding()]
  param(
    [Parameter(Mandatory=$true)] [string]$Prompt,
    [string]$Model = $env:OLLAMA_MODEL,
    [string]$Host  = $env:OLLAMA_HOST
  )
  if (-not $Model) { $Model = "llama3.1:8b" }
  if (-not $Host)  { $Host  = "http://127.0.0.1:11434" }

  $body = @{
    model = $Model
    prompt = $Prompt
    stream = $false
  } | ConvertTo-Json -Depth 4

  $resp = Invoke-RestMethod -Method POST -Uri "$Host/api/generate" -Body $body -ContentType "application/json" -TimeoutSec 120
  return $resp.response
}
Export-ModuleMember -Function Invoke-UStudioOllama
