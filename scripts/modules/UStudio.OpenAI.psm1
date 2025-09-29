
function Invoke-UStudioOpenAI {
  [CmdletBinding()]
  param(
    [Parameter(Mandatory=$true)] [string]$Prompt,
    [string]$Model = $env:OPENAI_MODEL,
    [string]$ApiBase = $null
  )
  if (-not $Model)   { $Model = "gpt-4o-mini" }
  if (-not $ApiBase) { $ApiBase = "https://api.openai.com/v1" }
  $key = $env:OPENAI_API_KEY
  if (-not $key) { throw "OPENAI_API_KEY is not set." }

  $body = @{
    model = $Model
    messages = @(@{ role="user"; content=$Prompt })
    temperature = 0.3
  } | ConvertTo-Json -Depth 6

  $headers = @{
    "Authorization" = "Bearer $key"
    "Content-Type"  = "application/json"
  }

  $resp = Invoke-RestMethod -Method POST -Uri "$ApiBase/chat/completions" -Headers $headers -Body $body -TimeoutSec 120
  return $resp.choices[0].message.content
}
Export-ModuleMember -Function Invoke-UStudioOpenAI
