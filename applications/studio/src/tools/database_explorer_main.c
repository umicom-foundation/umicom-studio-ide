/* Umicom Studio IDE | Sammy Hegab | Umicom Foundation | MIT */
#include <stdio.h>
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/database_explorer_commands.h"
int main(void){UmiStudioBootstrap *b=NULL;UmiStudioDatabaseExplorerCentre *c=NULL;UmiStudioDatabaseExplorerSnapshot s;UmiStatus st=umi_studio_bootstrap_create(&b);if(st==UMI_STATUS_OK)st=umi_studio_database_explorer_create(umi_studio_bootstrap_services(b),&c);if(st==UMI_STATUS_OK)st=umi_studio_database_explorer_snapshot(c,&s);if(st==UMI_STATUS_OK)(void)printf("Database Explorer: %s, %zu record(s), %zu table(s)\n",s.database.backend,s.database.records,s.database.tables);umi_studio_database_explorer_destroy(c);umi_studio_bootstrap_destroy(b);return st==UMI_STATUS_OK?0:1;}
