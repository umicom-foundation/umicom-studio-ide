/* Umicom Studio IDE | Operations views v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_OPERATIONS_VIEWS_V2_H
#define UMICOM_STUDIO_OPERATIONS_VIEWS_V2_H
#include "umicom/studio/operations_centre_v2.h"
typedef struct UmiStudioOperationsViewV2 {
    char view_id[UMI_OPERATIONS_ID_CAPACITY];
    char title[UMI_OPERATIONS_NAME_CAPACITY];
    char summary[UMI_OPERATIONS_TEXT_CAPACITY];
    size_t item_count;
    bool available;
} UmiStudioOperationsViewV2;
UmiStatus umi_studio_operations_view_v2_resolve(const UmiStudioOperationsCentreV2 *centre,const char *view_id,UmiStudioOperationsViewV2 *out_view);
#endif
