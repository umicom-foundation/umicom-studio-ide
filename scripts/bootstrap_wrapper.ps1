# -----------------------------------------------------------------------------
# Umicom Studio IDE
# PURPOSE: Project automation / helper script.
# Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
# Last updated: 2025-10-11
# -----------------------------------------------------------------------------
# Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
param()
& gradle wrapper --gradle-version 8.10.1
Write-Host "Wrapper generated. Use .\gradlew.bat assembleDebug"
