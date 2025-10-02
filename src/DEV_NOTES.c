/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/DEV_NOTES.c
 * PURPOSE: Developer reference (pure comments) — architecture, ownership rules,
 *          threading model, error handling, coding style, and integration tips.
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

/*
OVERVIEW
========
This file is intentionally compiled as C but contains only comments. It exists
to provide an *in-tree* reference for contributors and to fulfill packaging
constraints during staged delivery of the source tree.

The content below is organized in short, copy-pastable sections you can keep
open in your editor while working. Feel free to delete this file after you've
absorbed its content.

SECTIONS
--------
1.  Memory & Ownership
2.  UI Threading & Async I/O
3.  Error Handling Patterns
4.  Editor, Output, Search plumbing
5.  Workspace & Watcher integration
6.  Run/Build pipeline contracts
7.  Preferences & Session persistence
8.  Style & Formatting rules
9.  Roadmap (Next steps)

1) MEMORY & OWNERSHIP
---------------------
- All heap strings returned by APIs are newly allocated with g_strdup / GLib
  allocators. The *caller* owns and must free with g_free().
- Structures created with g_new0(...) are owned by the creating subsystem
  unless documented otherwise. When handing off ownership, function names
  include verbs like *_take_* or comments explicitly say "takes ownership".
- Gtk widgets follow GTK ownership rules (floating refs sink when packed).
- Use g_autofree and g_autoptr in local scopes where your compiler supports it.

Quick example:
    gchar *path = umi_path_normalize("a/../b.txt", base);
    if (!path) return FALSE;
    g_free(path);

2) UI THREADING & ASYNC I/O
---------------------------
- GTK widgets must be touched on the main thread.
- Long-running processes and file I/O happen asynchronously using
  GSubprocess + GDataInputStream and GLib async APIs.
- Signals from watchers or subprocess readers marshal back to the main loop.

3) ERROR HANDLING PATTERNS
--------------------------
- Public functions return gboolean for success/failure and optionally fill
  a GError** out-parameter. Callers may pass NULL when error details don't
  matter (e.g., autosave).
- Avoid g_error() in library-style code; prefer propagating errors up.

Example:
    gboolean ok = umi_editor_save(ed, &err);
    if (!ok) { umi_output_pane_append_line_err(ed->out, err->message); g_error_free(err); }

4) EDITOR, OUTPUT, SEARCH PLUMBING
----------------------------------
- UmiEditor holds the text buffer/view, a status strip, and a bottom notebook.
- UmiOutputPane acts as a simple console with append/append_line helpers and
  optional error channel formatting.
- UmiSearchPanel shells out to ripgrep (rg) and streams results using --vimgrep,
  parsing "file:line:col:text" and providing activation callbacks to jump
  the caret inside the editor.

5) WORKSPACE & WATCHER INTEGRATION
----------------------------------
- UmiWorkspace coordinates the file tree and a non-recursive GFileMonitor
  (future: recursive watch or fs events aggregation). On change, the tree
  refreshes opportunistically.
- Keep the watcher lightweight; expensive rescans should be throttled.

6) RUN/BUILD PIPELINE CONTRACTS
-------------------------------
- UmiRunConfig stores cwd/argv/env (JSON). Future phases may add profiles.
- UmiBuildRunner starts a subprocess, streams stdout/stderr to the output pane,
  and fires an exit callback. It is single-run; create a fresh runner for
  parallel jobs or teach it a queue model.
- Diagnostics parsing (gcc/clang/msvc) lives in problem_list; the editor wires
  a "Next Error" action to jump between rows.

7) PREFERENCES & SESSION PERSISTENCE
------------------------------------
- Preferences (config/ustudio.settings.json) control theme, font size,
  external tool paths, and autosave defaults.
- Session (config/session.json) preserves last opened file, cursor position,
  and autosave state. Autosave only writes when a current_file is known.

8) STYLE & FORMATTING RULES
---------------------------
- Use LF line endings, UTF-8, 2-space indents or clang-format defaults when
  available. Keep banner headers on all new files.
- Header guards use UMICOM_* pattern; prefer forward declarations for opaque
  structs and expose only necessary API surface.
- Comments: Favor "what/why" over "how". Document ownership and threading.

9) ROADMAP (NEXT STEPS)
-----------------------
- Introduce a Settings/Command bus to decouple UI from services.
- Add project templates and task runners for common languages.
- Integrate a linting pipeline and diagnostics squiggles in the editor
  (requires GtkSourceView or custom drawing layer).
- Make path_watcher recursive with platform-specific backends (inotify/FSEvents).
- Improve run_config to support multiple named profiles with environment overlays.

END OF NOTES
*/

/*
APPENDIX: TESTING CHECKLIST
---------------------------
- Launch app, open a file via the file tree, verify line numbers and caret jump.
- Run a command via Command Palette; ensure Output tab shows process logs.
- Trigger ripgrep search; double-click on a result to navigate.
- Toggle autosave in Preferences; edit a file and wait for timer to persist.
- Change theme to dark; confirm CSS overlay applies.
- Modify files under workspace root externally; confirm tree refreshes.
*/
