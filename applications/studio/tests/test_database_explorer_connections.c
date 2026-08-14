/* Umicom Studio IDE Tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/studio/database_explorer_connections.h"
int main(void){UmiStudioServices *services;UmiStudioDatabaseExplorerCentre *c=NULL;UmiDatabaseConnectionProfile p;assert(umi_studio_services_create(NULL,NULL,&services)==UMI_STATUS_OK);assert(umi_studio_database_explorer_create(services,&c)==UMI_STATUS_OK);assert(umi_studio_database_connection_add(c,"reporting","Reporting PostgreSQL",UMI_DATABASE_PROVIDER_POSTGRESQL,"db.example:5432","reporting","secret://database/reporting",1)==UMI_STATUS_OK);assert(umi_studio_database_connection_activate(c,"reporting")==UMI_STATUS_OK);assert(umi_database_connection_registry_active(&c->explorer->connections,&p)==UMI_STATUS_OK);assert(p.require_tls&&p.read_only&&strcmp(p.secret_reference,"secret://database/reporting")==0);umi_studio_database_explorer_destroy(c);umi_studio_services_destroy(services);return 0;}
