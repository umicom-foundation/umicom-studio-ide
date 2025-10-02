#!/usr/bin/env bash
# Commit helper: status -> add -A -> status -> commit -> push
set -e
git status
git add -A
git status
msg=${1:-"umicom: overlay apply (commit helper)"}
git commit -m "$msg" || true
git push origin HEAD || true
