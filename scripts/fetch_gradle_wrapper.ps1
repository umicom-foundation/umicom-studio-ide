param()
Set-Location (Join-Path $PSScriptRoot '..')
New-Item -ItemType Directory gradle\wrapper -Force | Out-Null
$jar='gradle/wrapper/gradle-wrapper.jar'
$uri='https://repo.gradle.org/gradle/libs-releases-local/org/gradle/gradle-wrapper/8.10.1/gradle-wrapper-8.10.1.jar'
try { Invoke-WebRequest -UseBasicParsing -Uri $uri -OutFile $jar } catch { Write-Host 'Falling back to: gradle wrapper --gradle-version 8.10.1'; & gradle wrapper --gradle-version 8.10.1 }
Write-Host 'Done. Try .\gradlew.bat --version'
