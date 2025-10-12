/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/proj_templates.c
 * PURPOSE: Write tiny Hello World templates for C/C++/Python
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "proj_templates.h"

static const char *C_MAIN =
  "#include <stdio.h>\n"
  "int main(){ puts(\"Hello, Umicom C!\"); return 0; }\n";

static const char *CPP_MAIN =
  "#include <iostream>\n"
  "int main(){ std::cout << \"Hello, Umicom C++!\\n\"; }\n";

static const char *PY_MAIN =
  "print('Hello, Umicom Python!')\n";

gboolean umi_tpl_write(const char *dir, UmiTemplateKind kind, GError **err){
  if(!dir) return FALSE;
  g_mkdir_with_parents(dir, 0755);
  const char *name = NULL, *src = NULL;
  switch(kind){
    case UMI_TPL_C:   name="main.c";  src=C_MAIN; break;
    case UMI_TPL_CPP: name="main.cpp";src=CPP_MAIN; break;
    case UMI_TPL_PY:  name="main.py"; src=PY_MAIN; break;
    default: return FALSE;
  }
  gchar *path = g_build_filename(dir, name, NULL);
  gboolean ok = g_file_set_contents(path, src, -1, err);
  g_free(path);
  return ok;
}
