/* Umicom Studio IDE Tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/studio/database_explorer_queries.h"
int main(void){UmiStudioServices *services;UmiStudioDatabaseExplorerCentre *c=NULL;UmiDatabaseQueryResult r;assert(umi_studio_services_create(NULL,NULL,&services)==UMI_STATUS_OK);assert(umi_studio_database_explorer_create(services,&c)==UMI_STATUS_OK);assert(umi_studio_database_explorer_set_write_mode(c,1)==UMI_STATUS_OK);assert(umi_studio_database_transaction_begin(c)==UMI_STATUS_OK);assert(umi_studio_database_query_execute(c,"SET theme dark",&r)==UMI_STATUS_OK);assert(umi_studio_database_transaction_commit(c)==UMI_STATUS_OK);assert(umi_studio_database_query_execute(c,"GET theme",&r)==UMI_STATUS_OK&&strcmp(r.preview,"dark")==0);umi_studio_database_explorer_destroy(c);umi_studio_services_destroy(services);return 0;}
