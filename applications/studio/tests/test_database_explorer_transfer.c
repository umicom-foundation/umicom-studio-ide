/* Umicom Studio IDE Tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/studio/database_explorer_transfer.h"
int main(void){UmiStudioServices *services;UmiStudioDatabaseExplorerCentre *c=NULL;UmiDatabaseTransferPlan p;UmiDatabaseBackupRecord b={0};assert(umi_studio_services_create(NULL,NULL,&services)==UMI_STATUS_OK);assert(umi_studio_database_explorer_create(services,&c)==UMI_STATUS_OK);assert(umi_database_transfer_plan_init(&p,"export",UMI_DATABASE_TRANSFER_EXPORT,UMI_DATABASE_FORMAT_JSON,"umicom_kv","records.json")==UMI_STATUS_OK);assert(umi_studio_database_transfer_add(c,&p)==UMI_STATUS_OK);(void)strcpy(b.id,"backup-1");(void)strcpy(b.path,"backup/studio.db");assert(umi_studio_database_backup_add(c,&b)==UMI_STATUS_OK);assert(umi_studio_database_backup_verify(c,"backup-1")==UMI_STATUS_OK);umi_studio_database_explorer_destroy(c);umi_studio_services_destroy(services);return 0;}
