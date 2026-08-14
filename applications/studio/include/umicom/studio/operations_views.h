/* Umicom Studio IDE | Operations views v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_OPERATIONS_VIEWS_H
#define UMICOM_STUDIO_OPERATIONS_VIEWS_H
#include "umicom/studio/operations_centre.h"
typedef struct UmiStudioOperationsView {
    char view_id[UMI_OPERATIONS_ID_CAPACITY];
    char title[UMI_OPERATIONS_NAME_CAPACITY];
    char summary[UMI_OPERATIONS_TEXT_CAPACITY];
    size_t item_count;
    bool available;
} UmiStudioOperationsView;
UmiStatus umi_studio_operations_view_resolve(const UmiStudioOperationsCentre *centre,const char *view_id,UmiStudioOperationsView *out_view);
#endif
