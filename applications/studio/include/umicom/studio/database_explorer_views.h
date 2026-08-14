/* Umicom Studio IDE | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_DATABASE_EXPLORER_VIEWS_H
#define UMICOM_STUDIO_DATABASE_EXPLORER_VIEWS_H
#include "umicom/studio/database_explorer_centre.h"
typedef enum UmiStudioDatabaseViewKind { UMI_STUDIO_DATABASE_VIEW_OVERVIEW=1, UMI_STUDIO_DATABASE_VIEW_CONNECTIONS=2, UMI_STUDIO_DATABASE_VIEW_SCHEMA=3, UMI_STUDIO_DATABASE_VIEW_QUERY=4, UMI_STUDIO_DATABASE_VIEW_HISTORY=5, UMI_STUDIO_DATABASE_VIEW_MIGRATIONS=6, UMI_STUDIO_DATABASE_VIEW_TRANSFER=7, UMI_STUDIO_DATABASE_VIEW_BACKUP=8 } UmiStudioDatabaseViewKind;
typedef struct UmiStudioDatabaseView { UmiStudioDatabaseViewKind kind; char id[UMI_DATABASE_ID_CAPACITY]; char title[UMI_DATABASE_NAME_CAPACITY]; char summary[UMI_DATABASE_TEXT_CAPACITY]; size_t row_count; uint64_t revision; } UmiStudioDatabaseView;
const char *umi_studio_database_view_kind_id(UmiStudioDatabaseViewKind kind);
UmiStatus umi_studio_database_view_build(const UmiStudioDatabaseExplorerCentre *centre,UmiStudioDatabaseViewKind kind,UmiStudioDatabaseView *out_view);
#endif
