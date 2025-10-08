#!/usr/bin/env python3
"""
Scans ./src for duplicate basenames across top-level and module subfolders.
Prefer keeping the modular copy (src/core|build|gui|editor|search|llm) and
mark the top-level src/*.c or src/*.h as delete candidates.

Outputs:
- tools/dupes_report.json
- tools/delete_candidates.txt
"""
from __future__ import annotations
import os, json, pathlib, sys
from collections import defaultdict

REPO = pathlib.Path(__file__).resolve().parents[1]
SRC  = REPO / "src"
TOOLS= REPO / "tools"
TOOLS.mkdir(parents=True, exist_ok=True)

MODULE_DIRS = {"core","build","gui","editor","search","llm"}

def is_source(p: pathlib.Path)->bool:
    if not p.is_file(): return False
    if p.suffix.lower() not in {".c",".h"}: return False
    # ignore obvious note files
    if p.name.upper().endswith("_NOTES.C"): return False
    return True

def walk_sources(root: pathlib.Path):
    for dirpath, dirnames, filenames in os.walk(root):
        for fn in filenames:
            p = pathlib.Path(dirpath) / fn
            if is_source(p):
                yield p

def classify(p: pathlib.Path)->str:
    # returns 'top' (src/ directly) or module folder (first child of src)
    rel = p.relative_to(SRC)
    parts = rel.parts
    if len(parts) == 1:
        return "top"
    return parts[0]

def choose_keep(paths):
    # paths: list[pathlib.Path] all with same basename
    # prefer module versions; if multiple, prefer "core" then others alphabetically
    module = [p for p in paths if classify(p) in MODULE_DIRS]
    if module:
        module.sort(key=lambda p: (classify(p)!="core", classify(p), str(p)))
        return module[0]
    # fallback: keep the first in sorted order
    return sorted(paths)[0]

def main():
    if not SRC.exists():
        print(f"ERROR: src folder not found at: {SRC}", file=sys.stderr)
        sys.exit(1)

    by_base = defaultdict(list)
    for p in walk_sources(SRC):
        by_base[p.name].append(p)

    dupes = {base: [str(p) for p in paths] for base, paths in by_base.items() if len(paths) > 1}

    report = {"root": str(REPO), "src": str(SRC), "dupes": dupes}
    (TOOLS/"dupes_report.json").write_text(json.dumps(report, indent=2), encoding="utf-8")

    deletes = []
    for base, paths in dupes.items():
        paths_p = [pathlib.Path(p) for p in paths]
        keep = choose_keep(paths_p)
        for p in paths_p:
            if p != keep:
                deletes.append(str(p))

    (TOOLS/"delete_candidates.txt").write_text("\n".join(deletes) + ("\n" if deletes else ""), encoding="utf-8")

    print(f"Scan complete. {len(dupes)} duplicate basenames found.")
    print(f" - Report: {TOOLS/'dupes_report.json'}")
    print(f" - Delete list: {TOOLS/'delete_candidates.txt'}")
    if deletes:
        print(f"Proposed to delete {len(deletes)} files (keep best match per group).")
    else:
        print("No deletion candidates detected.")

if __name__ == "__main__":
    main()
