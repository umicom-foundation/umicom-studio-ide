/* Umicom Studio IDE | Quality Centre views v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_QUALITY_VIEWS_H
#define UMICOM_STUDIO_QUALITY_VIEWS_H
#include "umicom/studio/quality_centre.h"
typedef enum UmiStudioQualityViewKind {
    UMI_STUDIO_QUALITY_VIEW_OVERVIEW = 1,
    UMI_STUDIO_QUALITY_VIEW_EVIDENCE,
    UMI_STUDIO_QUALITY_VIEW_ARCHITECTURE,
    UMI_STUDIO_QUALITY_VIEW_DUPLICATES,
    UMI_STUDIO_QUALITY_VIEW_COMPATIBILITY,
    UMI_STUDIO_QUALITY_VIEW_GATES,
    UMI_STUDIO_QUALITY_VIEW_REMEDIATION
} UmiStudioQualityViewKind;
typedef struct UmiStudioQualityView {
    UmiStudioQualityViewKind kind;
    char id[UMI_CODEGUARD_QUALITY_ID_CAPACITY];
    char title[UMI_CODEGUARD_QUALITY_ID_CAPACITY];
    char summary[UMI_CODEGUARD_QUALITY_TEXT_CAPACITY];
    size_t item_count;
    uint64_t revision;
} UmiStudioQualityView;
const char *umi_studio_quality_view_id(UmiStudioQualityViewKind kind);
UmiStatus umi_studio_quality_view_build(const UmiStudioQualityCentre *centre,UmiStudioQualityViewKind kind,UmiStudioQualityView *out_view);
#endif
