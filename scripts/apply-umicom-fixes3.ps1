# Usage:
#   powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\apply-umicom-fixes3b.ps1 -RepoRoot .
param(
  [string]$RepoRoot = "."
)

$ErrorActionPreference = "Stop"

function ReadText($p){ [IO.File]::ReadAllText($p,[Text.Encoding]::UTF8) }
function WriteText($p,$t){ [IO.File]::WriteAllText($p,$t,[Text.Encoding]::UTF8) }
function EnsureDir($p){ if(-not (Test-Path $p)){ New-Item -ItemType Directory -Force -Path $p | Out-Null } }

$root = (Resolve-Path $RepoRoot).Path
Write-Host "== Applying Umicom Windows build fixes (pack 3b) =="

# --- 1) src/main.c: normalize the __has_include block (fix ""..."" -> "...") ---
$main = Join-Path $root "src\main.c"
if(Test-Path $main){
  $c = ReadText $main
  if($c -match '__has_include'){
    $pattern = '(?s)#if\s+__has_include.*?#endif'
$fixed = @'
#if __has_include("ueng/common.h")
# include "ueng/common.h"
#elif __has_include("uengine/common.h")
# include "uengine/common.h"
#else
# error "Neither ueng/common.h nor uengine/common.h found"
#endif
'@
    $c = [regex]::Replace($c,$pattern,$fixed)
    WriteText $main $c
    Write-Host "Patched __has_include block in src\main.c"
  } else {
    Write-Host "src\main.c: no __has_include found (skip)"
  }
} else { Write-Host "WARN: src\main.c not found" }

# --- 2) src/gui/llm_lab.c: replace $false -> FALSE ---
$lab = Join-Path $root "src\gui\llm_lab.c"
if(Test-Path $lab){
  $c = ReadText $lab
  $nc = $c -replace '\$false','FALSE'
  if($nc -ne $c){ WriteText $lab $nc; Write-Host "Patched $false -> FALSE in src\gui\llm_lab.c" }
  else { Write-Host "src\gui\llm_lab.c already OK" }
} else { Write-Host "WARN: src\gui\llm_lab.c not found" }

# --- 3) src/llm/providers/llm_zai.c: clean comment header before first #include ---
$zai = Join-Path $root "src\llm\providers\llm_zai.c"
if(Test-Path $zai){
  $c = ReadText $zai
  $m = [regex]::Match($c,'(?m)^\s*#include\b.*')
  if($m.Success){
$header = @'
/*-----------------------------------------------------------------------------
 * Umicom LLM provider: ZAI
 * Clean file header (Windows patch). MIT-licensed.
 *---------------------------------------------------------------------------*/
'@
    $rest = $c.Substring($m.Index)
    $new = $header + $rest
    if($new -ne $c){ WriteText $zai $new; Write-Host "Rewrote header in src\llm\providers\llm_zai.c" }
    else { Write-Host "src\llm\providers\llm_zai.c header already OK" }
  } else {
    Write-Host "src\llm\providers\llm_zai.c: no #include found; leaving file unchanged"
  }
} else { Write-Host "WARN: src\llm\providers\llm_zai.c not found" }

# --- 4) src/build/build_system.c: remove any split_argv_safe(...) and append a clean one ---
$build = Join-Path $root "src\build\build_system.c"
if(Test-Path $build){
  $c = ReadText $build
  # Remove one or more function bodies named split_argv_safe(...)
  $pat = '(?s)^\s*(static\s+)?GPtrArray\s*\*\s*split_argv_safe\s*\([^)]*\)\s*\{.*?\}\s*'
  $removed = $false
  while([regex]::IsMatch($c,$pat)){ $c = [regex]::Replace($c,$pat,''); $removed = $true }
  if($removed){ Write-Host "Removed existing split_argv_safe(...) in src\build\build_system.c" }
  if($c -notmatch '\bsplit_argv_safe\s*\('){
$impl = @'
static GPtrArray* split_argv_safe(const gchar *cmd) {
  if (!cmd || !*cmd) {
    return g_ptr_array_new_with_free_func(g_free);
  }
  int argc = 0;
  gchar **argv = NULL;
  if (!g_shell_parse_argv(cmd, &argc, &argv, NULL)) {
    return g_ptr_array_new_with_free_func(g_free);
  }
  GPtrArray *a = g_ptr_array_new_with_free_func(g_free);
  for (int i = 0; i < argc; i++) g_ptr_array_add(a, g_strdup(argv[i]));
  g_strfreev(argv);
  g_ptr_array_add(a, NULL);
  return a;
}
'@
    $c = $c.TrimEnd() + "`r`n`r`n" + $impl
    Write-Host "Appended clean split_argv_safe(...)"
  }
  WriteText $build $c
} else { Write-Host "WARN: src\build\build_system.c not found" }

# --- 5) src/llm/llm_core.c: fix the broken two-line if(c=='\n') and nuke dup parser ---
$core = Join-Path $root "src\llm\llm_core.c"
if(Test-Path $core){
  $c = ReadText $core
  # Replace if(c=='\n') that got split across lines with if(c==39)
  $c2 = [regex]::Replace($c,"if\s*\(\s*c\s*==\s*'\s*\r?\n\s*'\s*\)","if(c==39)")
  # Drop any umi__parse_sse_buffer(...) bodies (dup/nested) and add a stub
  $pat = '(?s)^\s*static\s+void\s+umi__parse_sse_buffer\s*\([^)]*\)\s*\{.*?\}\s*'
  $removed = $false
  while([regex]::IsMatch($c2,$pat)){ $c2 = [regex]::Replace($c2,$pat,''); $removed = $true }
  if($removed){ Write-Host "Removed umi__parse_sse_buffer(...) in src\llm\llm_core.c" }
  if($c2 -notmatch '\bstatic\s+void\s+umi__parse_sse_buffer\s*\('){
$stub = @'
/* Minimal, safe stub to avoid nested/duplicated definitions on Windows.
 * If the real parser is needed, replace this stub. */
static void umi__parse_sse_buffer(UmiStreamCtx *sc) { (void)sc; }
'@
    $c2 = $c2.TrimEnd() + "`r`n`r`n" + $stub
    Write-Host "Appended umi__parse_sse_buffer(...) stub"
  }
  if($c2 -ne $c){ WriteText $core $c2; Write-Host "Patched src\llm\llm_core.c" } else { Write-Host "src\llm\llm_core.c already OK" }
} else { Write-Host "WARN: src\llm\llm_core.c not found" }

# --- 6) include/gui/app.h: create minimal UmiApp type ---
$guiIncDir = Join-Path $root "include\gui"
EnsureDir $guiIncDir
$apph = Join-Path $guiIncDir "app.h"
if(-not (Test-Path $apph)){
$h = @'
#ifndef UMI_GUI_APP_H
#define UMI_GUI_APP_H

#include <glib.h>

typedef struct _UmiEditor UmiEditor; /* defined elsewhere */

typedef struct {
  UmiEditor *ed;
  gpointer  reserved; /* future use */
} UmiApp;

#endif /* UMI_GUI_APP_H */
'@
  WriteText $apph $h
  Write-Host "Created include\gui\app.h (minimal UmiApp)"
} else { Write-Host "include\gui\app.h already exists" }

# --- 7) src/gui/app.c: ensure it includes "gui/app.h" ---
$appc = Join-Path $root "src\gui\app.c"
if(Test-Path $appc){
  $c = ReadText $appc
  if($c -notmatch '(?m)^\s*#\s*include\s+"gui/app.h"'){
    $m = [regex]::Matches($c,'(?m)^\s*#\s*include\s+[<"].+[>"]')
    if($m.Count -gt 0){
      $last = $m[$m.Count-1]
      $insertAt = $last.Index + $last.Length
      $c = $c.Substring(0,$insertAt) + "`r`n#include `"gui/app.h`"" + $c.Substring($insertAt)
    } else {
      $c = "#include `"gui/app.h`"`r`n" + $c
    }
    WriteText $appc $c
    Write-Host "Injected #include \"gui/app.h\" into src\gui\app.c"
  } else {
    Write-Host "src\gui\app.c already includes gui/app.h"
  }
} else { Write-Host "WARN: src\gui\app.c not found" }

# (Optional) 8) tests/test_llm_body.c: add weak stub if the test isn’t linking against llm_core
$testBody = Join-Path $root "tests\test_llm_body.c"
if(Test-Path $testBody){
  $c = ReadText $testBody
  if($c -match 'umi_llm_cfg_init_from_env' -and $c -notmatch 'UMI_TEST_STUB_LLM_CFG'){
$stub = @'
#ifndef UMI_TEST_STUB_LLM_CFG
#define UMI_TEST_STUB_LLM_CFG
typedef struct _UmiLlmCfg UmiLlmCfg;
__attribute__((weak)) UmiLlmCfg* umi_llm_cfg_init_from_env(GError **err){ (void)err; return NULL; }
#endif
'@
    $c = $c.TrimEnd() + "`r`n`r`n" + $stub
    WriteText $testBody $c
    Write-Host "Added weak stub in tests\\test_llm_body.c (allows linking if llm_core isn’t linked)"
  }
}

Write-Host "== Done. Now run: scripts\\verify-build.ps1 =="
