/* Umicom Studio IDE Tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/studio/database_explorer_centre.h"
int main(void){UmiDataServer *server=NULL;UmiStudioServices *services;UmiStudioDatabaseExplorerCentre *c=NULL;UmiStudioDatabaseExplorerSnapshot s;extern UmiStudioServices *umi_test_services_create(UmiDataServer *server);extern void umi_test_services_destroy(UmiStudioServices *services);assert(umi_data_server_create_memory(&server)==UMI_STATUS_OK);services=umi_test_services_create(server);assert(services!=NULL);assert(umi_studio_database_explorer_create(services,&c)==UMI_STATUS_OK);assert(umi_studio_database_explorer_activate(c,"schema")==UMI_STATUS_OK);assert(umi_studio_database_explorer_snapshot(c,&s)==UMI_STATUS_OK);assert(strcmp(s.active_pane,"schema")==0&&s.database.tables==1U);umi_studio_database_explorer_destroy(c);umi_test_services_destroy(services);umi_data_server_destroy(server);return 0;}
