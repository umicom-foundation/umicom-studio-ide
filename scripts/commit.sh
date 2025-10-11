#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# Umicom Studio IDE
# PURPOSE: Project automation / helper script.
# Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
# Last updated: 2025-10-11
# -----------------------------------------------------------------------------
# Commit helper: status -> add -A -> status -> commit -> push
set -e
git status
git add -A
git status
msg=${1:-"umicom: overlay apply (commit helper)"}
git commit -m "$msg" || true
git push origin HEAD || true
