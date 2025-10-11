/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/proj_templates.h
 * PURPOSE: Generate minimal project templates in a directory
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROJ_TEMPLATES_H
#define UMICOM_PROJ_TEMPLATES_H

#include <glib.h>

typedef enum { UMI_TPL_C, UMI_TPL_CPP, UMI_TPL_PY } UmiTemplateKind;

gboolean umi_tpl_write(const char *dir, UmiTemplateKind kind, GError **err);

#endif /* UMICOM_PROJ_TEMPLATES_H */
