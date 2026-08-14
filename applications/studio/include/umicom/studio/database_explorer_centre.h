/*-----------------------------------------------------------------------------
 * Umicom Studio IDE - Database Explorer composition centre
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_DATABASE_EXPLORER_CENTRE_H
#define UMICOM_STUDIO_DATABASE_EXPLORER_CENTRE_H
#include "umicom/studio/data.h"
#include "umicom/data/explorer.h"
typedef struct UmiStudioDatabaseExplorerCentre { UmiStudioServices *services; UmiDatabaseExplorer *explorer; char active_pane[UMI_DATABASE_ID_CAPACITY]; uint64_t revision; } UmiStudioDatabaseExplorerCentre;
typedef struct UmiStudioDatabaseExplorerSnapshot { UmiDatabaseExplorerSnapshot database; char active_pane[UMI_DATABASE_ID_CAPACITY]; int write_mode; uint64_t revision; } UmiStudioDatabaseExplorerSnapshot;
UmiStatus umi_studio_database_explorer_create(UmiStudioServices *services,UmiStudioDatabaseExplorerCentre **out_centre);
void umi_studio_database_explorer_destroy(UmiStudioDatabaseExplorerCentre *centre);
UmiStatus umi_studio_database_explorer_activate(UmiStudioDatabaseExplorerCentre *centre,const char *pane_id);
UmiStatus umi_studio_database_explorer_set_write_mode(UmiStudioDatabaseExplorerCentre *centre,int enabled);
UmiStatus umi_studio_database_explorer_snapshot(const UmiStudioDatabaseExplorerCentre *centre,UmiStudioDatabaseExplorerSnapshot *out_snapshot);
UmiDatabaseExplorer *umi_studio_database_explorer_service(UmiStudioDatabaseExplorerCentre *centre);
#endif
