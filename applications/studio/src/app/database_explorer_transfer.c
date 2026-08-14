/* Umicom Studio IDE | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/database_explorer_transfer.h"
UmiStatus umi_studio_database_transfer_add(UmiStudioDatabaseExplorerCentre *c,const UmiDatabaseTransferPlan *p){UmiStatus s;if(c==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_database_explorer_add_transfer(c->explorer,p);if(s==UMI_STATUS_OK)c->revision+=1U;return s;}
UmiStatus umi_studio_database_backup_add(UmiStudioDatabaseExplorerCentre *c,const UmiDatabaseBackupRecord *r){UmiStatus s;if(c==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_database_backup_catalog_add(&c->explorer->backups,r);if(s==UMI_STATUS_OK){c->explorer->revision+=1U;c->revision+=1U;}return s;}
UmiStatus umi_studio_database_backup_verify(UmiStudioDatabaseExplorerCentre *c,const char *id){UmiStatus s;if(c==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_database_backup_catalog_mark_verified(&c->explorer->backups,id);if(s==UMI_STATUS_OK){c->explorer->revision+=1U;c->revision+=1U;}return s;}
