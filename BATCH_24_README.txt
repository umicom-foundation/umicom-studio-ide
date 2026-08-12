Umicom Batch 24 — Unified Workbench View Integration and Build Repair
Created by: Sammy Hegab
Organisation: Umicom Foundation
Licence: MIT

BASELINE VERIFIED ON GITHUB
Framework: ab02d0109d1792df3df3d33e89e8a3f50c16fc13
Studio:    8caaa6d6931a57b84b465f78fad35505cc227d89

WHY THIS BATCH EXISTS
Batch 23 source files were committed successfully, but the Batch 23 CMake merge
files were outside the user's accepted umicom-studio/framework tree and were
therefore not merged. Current CI consequently compiles the old Framework UI
source list and fails when workbench.c references the new Batch 23 services.

BATCH 24 DOES TWO THINGS
1. Completes the missing Batch 23 build/test/GTK integration and aligns the
   Framework/Studio code versions to Framework 0.6.0 and Studio 0.15.0.
2. Adds real reusable view-model presentation:
   - Framework UmiUiViewPresentation snapshot API.
   - GTK4 generic view-model renderer.
   - Headless generic view-model renderer.
   - Correct centre-pane routing so Editor no longer falls through to sidebar.
   - Studio factories for Explorer, Search, Source Control, Run/Debug, Testing,
     Designer, Applications, Framework and AI/AuthorEngine.
   - Live view properties from existing Studio services where available.
   - New Framework and Studio tests.

IMPORTANT MERGE NOTE
This ZIP intentionally has ONE top-level folder only:
    umicom-studio/

Framework content is only here:
    umicom-studio/framework/

Two files are named *.batch24.merge.txt because the current CMakeLists.txt files are
large and Batch 23 did not contain complete replacements:
    framework/CMakeLists.txt.batch24.merge.txt
    applications/studio/CMakeLists.txt.batch24.merge.txt

Compare those patch files with the real CMakeLists.txt files and merge the shown
lines into the existing CMakeLists.txt. Do not keep the *.merge.txt files in the
repository unless you specifically want them documented.

The root umicom-studio/CMakeLists.txt IS a complete replacement because the
current GitHub file exactly matches the retained source baseline used here.
