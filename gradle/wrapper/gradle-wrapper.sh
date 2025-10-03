#!/usr/bin/env sh
# relay to gradle if jar exists, else hint
JAR="$(dirname "$0")/gradle-wrapper.jar"
if [ ! -f "$JAR" ]; then echo "Missing gradle-wrapper.jar. Run scripts/fetch_gradle_wrapper.sh"; exit 1; fi
exec java -jar "$JAR" "$@"
