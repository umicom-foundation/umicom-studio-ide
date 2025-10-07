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