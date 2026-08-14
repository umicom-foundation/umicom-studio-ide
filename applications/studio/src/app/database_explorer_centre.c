/*-----------------------------------------------------------------------------
 * Umicom Studio IDE - Database Explorer composition centre
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/database_explorer_centre.h"
#include <stdlib.h>
#include <string.h>
UmiStatus umi_studio_database_explorer_create(UmiStudioServices *services,UmiStudioDatabaseExplorerCentre **out){UmiStudioDatabaseExplorerCentre *c;UmiStatus s;if(services==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;c=calloc(1U,sizeof(*c));if(c==NULL)return UMI_STATUS_OUT_OF_MEMORY;c->explorer=calloc(1U,sizeof(*c->explorer));if(c->explorer==NULL){free(c);return UMI_STATUS_OUT_OF_MEMORY;}c->services=services;s=umi_database_explorer_init(c->explorer,umi_studio_services_data_server(services));if(s==UMI_STATUS_OK)s=umi_database_copy_text(c->active_pane,sizeof(c->active_pane),"overview");if(s!=UMI_STATUS_OK){umi_studio_database_explorer_destroy(c);return s;}c->revision=1U;*out=c;return UMI_STATUS_OK;}
void umi_studio_database_explorer_destroy(UmiStudioDatabaseExplorerCentre *c){if(c==NULL)return;free(c->explorer);free(c);}
UmiStatus umi_studio_database_explorer_activate(UmiStudioDatabaseExplorerCentre *c,const char *id){UmiStatus s;if(c==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_database_copy_text(c->active_pane,sizeof(c->active_pane),id);if(s==UMI_STATUS_OK)c->revision+=1U;return s;}
UmiStatus umi_studio_database_explorer_set_write_mode(UmiStudioDatabaseExplorerCentre *c,int enabled){if(c==NULL||c->explorer==NULL)return UMI_STATUS_INVALID_ARGUMENT;c->explorer->queries.policy.allow_writes=enabled?1:0;c->explorer->queries.policy.allow_schema_changes=enabled?1:0;c->revision+=1U;return UMI_STATUS_OK;}
UmiStatus umi_studio_database_explorer_snapshot(const UmiStudioDatabaseExplorerCentre *c,UmiStudioDatabaseExplorerSnapshot *out){UmiStatus s;if(c==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(out,0,sizeof(*out));s=umi_database_explorer_snapshot(c->explorer,&out->database);if(s==UMI_STATUS_OK)s=umi_database_copy_text(out->active_pane,sizeof(out->active_pane),c->active_pane);out->write_mode=c->explorer->queries.policy.allow_writes;out->revision=c->revision;return s;}
UmiDatabaseExplorer *umi_studio_database_explorer_service(UmiStudioDatabaseExplorerCentre *c){return c!=NULL?c->explorer:NULL;}
