UMICOM BATCH 25 — UNIFIED WORKBENCH BUILD-INTEGRATION REPAIR
============================================================

Created by: Sammy Hegab
Organisation: Umicom Foundation
Licence: MIT

WHY THIS REPAIR IS REQUIRED
---------------------------
GitHub was checked after Batch 24.

Verified heads before Batch 25:
  Framework: 5d208c3c2b683ba61e1873493a49dea56781e707
  Studio:    5f3f709fb954cdcd85fc08fa2407dffea4516225

The Batch 24 source/header files are present, but the intended CMake changes were
left in CMakeLists.txt.batch24.merge.txt guide files instead of being merged into
the real CMakeLists.txt files. GitHub Studio CI therefore still fails at link time
with undefined unified-workbench symbols.

Batch 25 does NOT add another feature layer. It first restores a trustworthy,
buildable baseline.

IMPORTANT PACKAGING RULE
------------------------
This archive has exactly one top-level project folder: umicom-studio.
The Framework repair lives under umicom-studio/framework through the applicator.
Nothing that matters to the merge is stored outside umicom-studio.

The helper under tools/batch25 is TRANSIENT. Run it, verify the real project
files, then delete tools/batch25 before committing.

PERMANENT REPOSITORY CHANGES MADE BY THE HELPER
-----------------------------------------------
Framework:
  MODIFY  framework/CMakeLists.txt
  DELETE  framework/CMakeLists.txt.batch24.merge.txt

Studio:
  MODIFY  applications/studio/CMakeLists.txt
  MODIFY  VERSION_LOCK.json
  DELETE  applications/studio/CMakeLists.txt.batch24.merge.txt

The real CMake files will be changed in-place. No *.merge.txt replacement is
created in Batch 25.

STEP 1 — COPY THIS BATCH INTO STUDIO
------------------------------------
Copy the archive's umicom-studio content into:

  C:\Dev\umicom\umicom-studio

Do NOT commit yet.

STEP 2 — APPLY FRAMEWORK REPAIR FIRST
-------------------------------------
PowerShell:

  cd C:\Dev\umicom\umicom-studio
  cmake -DSTAGE=framework -P tools/batch25/apply_batch25.cmake

Inspect:

  git -C framework diff -- CMakeLists.txt
  git -C framework status --short

The Framework CMake diff must show:
  * project version 0.6.0
  * all Batch 23/24 unified-workbench UI source files compiled
  * headless view-model renderer compiled
  * GTK4 Activity Bar/view-container/breadcrumb/quick-access/keybinding/view renderer compiled
  * Batch 23/24 Framework UI tests registered
  * old CMakeLists.txt.batch24.merge.txt deleted

STEP 3 — BUILD AND TEST FRAMEWORK
---------------------------------
From the Framework folder:

  cd C:\Dev\umicom\umicom-studio\framework
  cmake -S . -B build-batch25 -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DUMICOM_BUILD_GTK4_ADAPTER=OFF
  cmake --build build-batch25 --parallel 2
  ctest --test-dir build-batch25 --output-on-failure

STOP if build or CTest fails. Do not proceed to Studio.

STEP 4 — COMMIT AND PUSH FRAMEWORK
----------------------------------
Still inside framework:

  git add -A
  git commit -m "fix(framework): complete unified workbench CMake integration"
  git push

Record the commit if you want to inspect it:

  git rev-parse HEAD

STEP 5 — APPLY STUDIO REPAIR
----------------------------
Return to Studio root. The Studio stage deliberately runs AFTER the Framework
commit, because VERSION_LOCK.json must record the NEW Framework commit ID.

  cd C:\Dev\umicom\umicom-studio
  cmake -DSTAGE=studio -P tools/batch25/apply_batch25.cmake

The Studio stage automatically reads:
  git -C framework rev-parse HEAD

and writes that exact commit into VERSION_LOCK.json.

Inspect:

  git diff -- applications/studio/CMakeLists.txt VERSION_LOCK.json
  git status --short

The Studio CMake diff must show:
  * Studio application version 0.15.0
  * workbench_shell_catalogue.c compiled
  * workbench_views.c compiled
  * Framework-backed workbench_window.c included by umicom-studio-ide
  * Umicom::ui_gtk4 linked by umicom-studio-ide
  * product compile definitions changed from minor 14 to 15
  * Batch 23/24 unified-workbench Studio tests registered
  * old applications/studio/CMakeLists.txt.batch24.merge.txt deleted

VERSION_LOCK.json must show:
  studio_version: 0.15.0
  framework_version: 0.6.0
  framework_commit: the Framework commit created in Step 4
  studio_parent_base_commit: the Studio HEAD that existed before Batch 25

STEP 6 — STRUCTURAL CHECK
-------------------------
From Studio root:

  cmake -DSTAGE=check -P tools/batch25/apply_batch25.cmake

Expected final line:

  -- Batch 25 structural validation: PASS

STEP 7 — BUILD AND TEST STUDIO HEADLESS
---------------------------------------
From Studio root:

  cmake -S . -B build-batch25 -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DUMICOM_STUDIO_BUILD_GTK=OFF -DUMICOM_ENABLE_STRICT_WARNINGS=ON
  cmake --build build-batch25 --parallel 2
  ctest --test-dir build-batch25 --output-on-failure

STOP if any command fails.

STEP 8 — OPTIONAL LOCAL GTK4/UCRT64 ACCEPTANCE
----------------------------------------------
If your MSYS2 UCRT64 GTK4 environment is already configured, perform the normal
full GTK build before committing Studio. Batch 25 also adds the Batch 23/24 GTK4
adapter implementation files to the real Framework target, which headless CI
cannot compile because GTK is deliberately OFF there.

STEP 9 — REMOVE TRANSIENT BATCH TOOL
------------------------------------
Do this BEFORE git add:

  Remove-Item -Recurse -Force .\tools\batch25

The tools/batch25 folder is a delivery helper, not permanent product source.

STEP 10 — COMMIT AND PUSH STUDIO
--------------------------------
From Studio root:

  git add -A
  git commit -m "fix(studio): complete unified workbench CMake integration"
  git push

After push, wait for GitHub Actions. The previous failure occurred during the
compile/link stage; the next run must get beyond compilation and execute tests.

DO NOT START BATCH 26 UNTIL BATCH 25 CI IS GREEN.
