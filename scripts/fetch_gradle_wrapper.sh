#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p gradle/wrapper
JAR=gradle/wrapper/gradle-wrapper.jar
URL="https://services.gradle.org/distributions/gradle-8.10.1-bin.zip"
echo "Downloading wrapper jar from Gradle distribution..."
# Fetch jar from known wrapper repo (fallback to gradle init if available)
if command -v curl >/dev/null 2>&1; then
  curl -fsSL https://repo.gradle.org/gradle/libs-releases-local/org/gradle/gradle-wrapper/8.10.1/gradle-wrapper-8.10.1.jar -o "$JAR" || true
fi
if [ ! -f "$JAR" ]; then
  echo "Falling back to 'gradle wrapper' to generate wrapper files..."
  gradle wrapper --gradle-version 8.10.1
fi
echo "Done. Try ./gradlew --version"
