#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# Umicom Studio IDE
# PURPOSE: Project automation / helper script.
# Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
# Last updated: 2025-10-11
# -----------------------------------------------------------------------------
# Deletes files listed in a text file. Lines starting with '#' are ignored.
# Usage:
#   ./tools/cleanup_dupes.sh tools/delete_candidates.txt 1   # dry run
#   ./tools/cleanup_dupes.sh tools/delete_candidates.txt 0   # delete
set -euo pipefail

if [[ $# -lt 1 ]]; then
  echo "Usage: $0 <list-path> [dry-run:1|0]" >&2
  exit 1
fi

LIST="$1"
DRY="${2:-1}"  # default dry-run

if [[ ! -f "$LIST" ]]; then
  echo "List not found: $LIST" >&2
  exit 1
fi

mapfile -t FILES < <(grep -v '^\s*$' "$LIST" | grep -v '^\s*#' | sed 's/\r$//')

echo "Candidate files (${#FILES[@]}):"
for f in "${FILES[@]}"; do
  echo "  $f"
done

if [[ "${DRY}" == "1" ]]; then
  echo
  echo "Dry run only. No files were deleted."
  exit 0
fi

read -rp $'\nType YES to permanently delete the files above: ' CONFIRM
if [[ "$CONFIRM" != "YES" ]]; then
  echo "Aborted."
  exit 0
fi

deleted=0 missing=0 failed=0
for f in "${FILES[@]}"; do
  if [[ -e "$f" ]]; then
    if rm -f -- "$f"; then
      echo "Deleted: $f"
      ((deleted++))
    else
      echo "Failed:  $f" >&2
      ((failed++))
    fi
  else
    echo "Missing: $f"
    ((missing++))
  fi
done

echo
echo "Summary: deleted=$deleted, missing=$missing, failed=$failed"
