/* Umicom Studio IDE | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_DATABASE_EXPLORER_TRANSFER_H
#define UMICOM_STUDIO_DATABASE_EXPLORER_TRANSFER_H
#include "umicom/studio/database_explorer_centre.h"
UmiStatus umi_studio_database_transfer_add(UmiStudioDatabaseExplorerCentre *centre,const UmiDatabaseTransferPlan *plan);
UmiStatus umi_studio_database_backup_add(UmiStudioDatabaseExplorerCentre *centre,const UmiDatabaseBackupRecord *record);
UmiStatus umi_studio_database_backup_verify(UmiStudioDatabaseExplorerCentre *centre,const char *backup_id);
#endif
