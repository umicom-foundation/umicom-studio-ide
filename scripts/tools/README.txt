Umicom Studio IDE — Duplicate Cleanup Tools (v2)
====================================================

Files included
--------------
tools/find_dupes.py            # scans src/ for duplicate basenames and proposes deletions
tools/cleanup_dupes.ps1        # Windows PowerShell deleter (supports -DryRun and -WhatIf alias)
tools/cleanup_dupes.sh         # Bash deleter (supports dry-run flag)
tools/delete_candidates.txt    # (created by find_dupes.py)
tools/dupes_report.json        # (created by find_dupes.py)

Quick start
-----------
1) Generate lists:
   python tools/find_dupes.py

2) Review:
   - tools/dupes_report.json
   - tools/delete_candidates.txt

3) Dry-run delete (Windows):
   powershell -ExecutionPolicy Bypass -File tools/cleanup_dupes.ps1 -ListPath tools/delete_candidates.txt -DryRun

   or Bash:
   ./tools/cleanup_dupes.sh tools/delete_candidates.txt 1

4) Actually delete (after backup / commit):
   powershell -ExecutionPolicy Bypass -File tools/cleanup_dupes.ps1 -ListPath tools/delete_candidates.txt -Force

   or:
   ./tools/cleanup_dupes.sh tools/delete_candidates.txt 0

Notes
-----
- The PowerShell script accepts -WhatIf as an alias for -DryRun.
- The Bash script expects "1" for dry-run, "0" for deleting.
- You can edit tools/delete_candidates.txt before running the deleter.
