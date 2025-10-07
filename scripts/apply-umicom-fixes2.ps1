Param([string]$RepoRoot = ".")

$ErrorActionPreference = "Stop"
Write-Host "== Applying Umicom Windows build fixes (pack 2) =="

function Patch-File($path, $from, $to){
  if(Test-Path $path){
    $text = Get-Content $path -Raw
    $new = [regex]::Replace($text, $from, $to, [System.Text.RegularExpressions.RegexOptions]::Singleline)
    if($new -ne $text){
      Set-Content $path $new -NoNewline
      Write-Host "Patched $path"
    } else {
      Write-Host "No change for $path"
    }
  } else { Write-Host "Skip $path (not found)" }
}

# 1) src/main.c – include shim so either ueng/ or uengine/ works
$main = Join-Path $RepoRoot "src\main.c"
$pattern1 = '(?m)^\s*#\s*include\s+"(?:ueng|uengine)/common\.h".*$'
$replacement1 = @"
#if __has_include(""ueng/common.h"")
#include ""ueng/common.h""
#elif __has_include(""uengine/common.h"")
#include ""uengine/common.h""
#else
# error ""Neither ueng/common.h nor uengine/common.h found""
#endif
"@
Patch-File $main $pattern1 $replacement1

# 2) src/build/build_system.c – canonical split_argv_safe and de-dup
$buildsys = Join-Path $RepoRoot "src\build\build_system.c"
$pattern2 = 'static\s+GPtrArray\s*\*\s*split_argv_safe\s*\(\s*const\s+char\s*\*\s*cmd\s*\)\s*\{.*?\n\}'
$replacement2 = @"
static GPtrArray *split_argv_safe(const char *cmd) {
  int argc = 0;
  char **argv = NULL;
  GError *gerr = NULL;

  if (!cmd || !*cmd) {
    GPtrArray *empty = g_ptr_array_new_with_free_func(g_free);
    g_ptr_array_add(empty, NULL);
    return empty;
  }
  if (!g_shell_parse_argv(cmd, &argc, &argv, &gerr)) {
    if (gerr) { g_error_free(gerr); }
    GPtrArray *empty = g_ptr_array_new_with_free_func(g_free);
    g_ptr_array_add(empty, NULL);
    return empty;
  }
  GPtrArray *a = g_ptr_array_new_with_free_func(g_free);
  for (int i = 0; i < argc; ++i) g_ptr_array_add(a, g_strdup(argv[i]));
  g_strfreev(argv);
  g_ptr_array_add(a, NULL);
  return a;
}
"@
Patch-File $buildsys $pattern2 $replacement2

# 3) src/llm/providers/llm_zai.c – fix malformed block comment header
$zai = Join-Path $RepoRoot "src\llm\providers\llm_zai.c"
$pattern3 = '^\s*/\*.*?\*/'
$replacement3 = @"
/* Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
 * --------------------------------------------------------------------------- */
"@
Patch-File $zai $pattern3 $replacement3

# 4) src/gui/llm_lab.c – GTK4-friendly main loop spin
$lab = Join-Path $RepoRoot "src\gui\llm_lab.c"
$pattern4 = 'while\s*\(\s*gtk_events_pending\s*\(\s*\)\s*\)\s*gtk_main_iteration\s*\(\s*\)\s*;?'
$replacement4 = 'while (g_main_context_pending(NULL)) g_main_context_iteration(NULL, $false);'
Patch-File $lab $pattern4 $replacement4

# 5) src/llm/llm_core.c – replace broken SSE parser body
$core = Join-Path $RepoRoot "src\llm\llm_core.c"
$pattern5 = 'static\s+void\s+umi__parse_sse_buffer\s*\(\s*UmiStreamCtx\s*\*\s*sc\s*\)\s*\{.*?\n\}\s*'
$replacement5 = @"
static void umi__parse_sse_buffer(UmiStreamCtx *sc) {
  if (!sc || !sc->buf) return;

  gchar **lines = g_strsplit(sc->buf->str, ""\n"", 0);
  if (!lines) return;

  for (guint i = 0; lines[i]; i++) {
    const gchar *s = lines[i];
    if (!s || !*s) continue;

    if (g_str_has_prefix(s, ""data: "")) {
      const gchar *payload = s + 6;
      umi__handle_sse_line(sc, payload);
    }
  }
  g_strfreev(lines);
  g_string_set_size(sc->buf, 0);
}
"@
Patch-File $core $pattern5 $replacement5

# 6) include/editor/editor.h – create minimal public header if missing
$editorHeader = Join-Path $RepoRoot "include\editor\editor.h"
if (-not (Test-Path $editorHeader)) {
  New-Item -ItemType Directory -Force (Split-Path $editorHeader) | Out-Null
  @"
#pragma once
#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _UmiEditor UmiEditor;

void umi_editor_open_file(UmiEditor *ed, const char *path, GError **error);
void umi_editor_save(UmiEditor *ed, GError **error);

#ifdef __cplusplus
}
#endif
"@ | Set-Content $editorHeader -NoNewline
  Write-Host "Created $editorHeader"
} else {
  Write-Host "$editorHeader already exists"
}

# 7) include/ueng/common.h – compatibility shim
$uengShim = Join-Path $RepoRoot "include\ueng\common.h"
if (-not (Test-Path $uengShim)) {
  New-Item -ItemType Directory -Force (Split-Path $uengShim) | Out-Null
  @"
#pragma once
#if __has_include(""uengine/common.h"")
#include ""uengine/common.h""
#else
#include <glib.h>
/* Fallback: add declarations here if needed */
#endif
"@ | Set-Content $uengShim -NoNewline
  Write-Host "Created $uengShim"
} else {
  Write-Host "$uengShim already exists"
}

Write-Host "== Done. Run scripts\verify-build.ps1 again =="
exit 0
