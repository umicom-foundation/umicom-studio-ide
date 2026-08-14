/* Umicom Studio IDE Tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/studio/database_explorer_views.h"
int main(void){UmiDataServer *server=NULL;UmiStudioServices *services;UmiStudioDatabaseExplorerCentre *c=NULL;UmiStudioDatabaseView v;extern UmiStudioServices *umi_test_services_create(UmiDataServer *server);extern void umi_test_services_destroy(UmiStudioServices *services);assert(umi_data_server_create_memory(&server)==UMI_STATUS_OK);services=umi_test_services_create(server);assert(umi_studio_database_explorer_create(services,&c)==UMI_STATUS_OK);assert(umi_studio_database_view_build(c,UMI_STUDIO_DATABASE_VIEW_SCHEMA,&v)==UMI_STATUS_OK);assert(strcmp(v.id,"schema")==0&&v.row_count==1U&&strstr(v.summary,"memory")!=NULL);umi_studio_database_explorer_destroy(c);umi_test_services_destroy(services);umi_data_server_destroy(server);return 0;}
