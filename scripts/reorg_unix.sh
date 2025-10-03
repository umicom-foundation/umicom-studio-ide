#!/usr/bin/env bash
# Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
# Purpose: Repository reorganization and validation helpers.

set -euo pipefail
DRY=0
STRATEGY="skip"  # skip | overwrite | ts

for arg in "$@"; do
  case "$arg" in
    --dry-run) DRY=1 ;;
    --strategy=*) STRATEGY="${arg#*=}" ;;
  esac
done

mkdir -p templates/android/full-mm templates/android/enterprise
mkdir -p templates/web/{react-vite,react-vite-ts,vue-vite,vue-vite-ts,svelte-vite,sveltekit}
mkdir -p templates/web/node/{express-api,express-jwt}
mkdir -p templates/web/vanilla
mkdir -p templates/wt/{hello,crud,crud-sqlite,routing,auth-stub,rest-json,file-upload,charts}

move_git () {
  SRC="$1"; DST="$2"
  if [ ! -e "$SRC" ]; then echo "Skip (missing): $SRC"; return 0; fi
  DDIR="$(dirname "$DST")"; mkdir -p "$DDIR"
  if [ -e "$DST" ]; then
    case "$STRATEGY" in
      skip) echo "Skip (dest exists): $SRC -> $DST"; return 0 ;;
      overwrite) 
        if [ "$DRY" = "1" ]; then echo "[DRY] overwrite: $SRC -> $DST"; return 0; fi
        git rm -r --cached --ignore-unmatch -- "$DST" >/dev/null 2>&1 || true
        rm -rf "$DST"
        ;;
      ts)
        TS="$(date +%Y%m%d_%H%M%S)"
        DST="$(dirname "$DST")/incoming_${TS}_$(basename "$SRC")"
        ;;
    esac
  fi
  if [ "$DRY" = "1" ]; then echo "[DRY] git mv $SRC $DST"; else git mv "$SRC" "$DST"; fi
  echo "Moved $SRC -> $DST"
}

# Android
move_git app templates/android/full-mm/app
move_git feature templates/android/full-mm/feature
move_git lib templates/android/full-mm/lib
move_git gradle templates/android/full-mm/gradle
move_git gradlew templates/android/full-mm/gradlew
move_git gradlew.bat templates/android/full-mm/gradlew.bat
move_git build.gradle.kts templates/android/full-mm/build.gradle.kts
move_git gradle.properties templates/android/full-mm/gradle.properties

# Web
move_git react-vite templates/web/react-vite
move_git react-vite-ts templates/web/react-vite-ts
move_git vue-vite templates/web/vue-vite
move_git vue-vite-ts templates/web/vue-vite-ts
move_git svelte-vite templates/web/svelte-vite
move_git sveltekit templates/web/sveltekit
move_git node-express-api templates/web/node/express-api
move_git node-express-advanced templates/web/node/express-jwt
move_git web-vanilla templates/web/vanilla

# Wt
move_git wt-hello templates/wt/hello
move_git wt-crud templates/wt/crud
move_git wt-crud-sqlite templates/wt/crud-sqlite
move_git wt-routing templates/wt/routing
move_git wt-auth-stub templates/wt/auth-stub
move_git wt-rest-json templates/wt/rest-json
move_git wt-file-upload templates/wt/file-upload
move_git wt-charts templates/wt/charts

echo "Done. Review changes with: git status"
