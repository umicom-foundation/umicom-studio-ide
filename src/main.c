/*-----------------------------------------------------------------------------
 * Umicom AuthorEngine AI (uaengine)
 * File: src/main.c
 * PURPOSE: Command-line front-end (dispatcher + light helpers)
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 15-09-2025
 * License: MIT
 *---------------------------------------------------------------------------*/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
/* ============================ QUICK START FOR NEW CONTRIBUTORS ============================
   This file implements the uaengine CLI. The program is organized as:
     - Small helpers (path/tool detection, YAML mini-parser)
     - Command handlers: cmd_init, cmd_ingest, cmd_build, cmd_export, cmd_open, cmd_serve,
   cmd_doctor, cmd_publish
     - main(): routes argv[1] to one of the command handlers
   Build flow (high level):
     1) 'uaengine init'  -> creates folders and a boilerplate book.yaml
     2) 'uaengine build' -> optionally ingests/normalizes content and packs workspace/book-draft.md
     3) 'uaengine export'-> runs Pandoc to produce HTML (and optionally PDF), writing logs next to
   outputs 4) 'uaengine open'  -> opens the generated site index in your browser 5) 'uaengine serve'
   -> (optional) serves the site folder locally

   Windows specifics:
     - We pass ABSOLUTE paths to Pandoc to avoid working-directory surprises.
     - We use '--metadata-file meta.yaml' instead of inline '--metadata' to avoid quoting issues.
     - On export failure, a short warning is printed and the first lines of pandoc_err.txt are
   echoed.
   ========================================================================================= */
#if __has_include("ueng/common.h")
# include "ueng/common.h"
#elif __has_include("uengine/common.h")
# include "uengine/common.h"
#else
# error "Neither ueng/common.h nor uengine/common.h found"
#endif