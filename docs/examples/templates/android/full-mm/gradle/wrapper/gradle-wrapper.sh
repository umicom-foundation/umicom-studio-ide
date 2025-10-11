#!/usr/bin/env sh
# -----------------------------------------------------------------------------
# Umicom Studio IDE
# PURPOSE: Project automation / helper script.
# Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
# Last updated: 2025-10-11
# -----------------------------------------------------------------------------
# relay to gradle if jar exists, else hint
JAR="$(dirname "$0")/gradle-wrapper.jar"
if [ ! -f "$JAR" ]; then echo "Missing gradle-wrapper.jar. Run scripts/fetch_gradle_wrapper.sh"; exit 1; fi
exec java -jar "$JAR" "$@"
