/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * PURPOSE: Project-wide small fixes to resolve current build errors
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-07 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_EDITOR_H
#define UMI_EDITOR_H

#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _UmiEditor UmiEditor;

/* Construction */
UmiEditor *umi_editor_new(void);
/* Access to the main widget for packing into layouts */
GtkWidget *umi_editor_widget(UmiEditor *ed);

/* File operations — declarations were missing which caused implicit-decl errors */
void umi_editor_open_file(UmiEditor *ed, const char *path, GError **err);
void umi_editor_save     (UmiEditor *ed, GError **err);
void umi_editor_save_as  (UmiEditor *ed, GError **err);

#ifdef __cplusplus
}
#endif

#endif /* UMI_EDITOR_H */
