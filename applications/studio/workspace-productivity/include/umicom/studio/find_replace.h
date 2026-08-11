/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/workspace-productivity/include/umicom/studio/find_replace.h
 *
 * PURPOSE:
 *   Provide bounded literal find/replace operations and search options.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is part of Studio workspace/editor productivity. The implementation
 * uses bounded C23 data structures and explicit ownership so the behaviour is
 * easy to inspect, test and later connect to the main Studio workbench.
 */

#ifndef UMICOM_STUDIO_FIND_REPLACE_H
#define UMICOM_STUDIO_FIND_REPLACE_H

#include <stdbool.h>
#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/studio/workspace_document.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioFindReplace {
    char search[UMI_STUDIO_WORKSPACE_NAME_CAPACITY];
    char replacement[UMI_STUDIO_WORKSPACE_NAME_CAPACITY];
    bool case_sensitive;
    bool whole_word;
    bool regular_expression;
    bool selection_only;
} UmiStudioFindReplace;

void umi_studio_find_replace_init(UmiStudioFindReplace *operation);

UmiStatus umi_studio_find_replace_set(
    UmiStudioFindReplace *operation,
    const char *search,
    const char *replacement
);

UmiStatus umi_studio_find_count(
    const char *text,
    const UmiStudioFindReplace *operation,
    size_t *count
);

UmiStatus umi_studio_replace_all(
    const char *text,
    const UmiStudioFindReplace *operation,
    char *output,
    size_t output_capacity,
    size_t *replacement_count
);

#ifdef __cplusplus
}
#endif

#endif
