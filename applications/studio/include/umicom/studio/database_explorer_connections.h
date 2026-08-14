/* Umicom Studio IDE | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_DATABASE_EXPLORER_CONNECTIONS_H
#define UMICOM_STUDIO_DATABASE_EXPLORER_CONNECTIONS_H
#include "umicom/studio/database_explorer_centre.h"
UmiStatus umi_studio_database_connection_add(UmiStudioDatabaseExplorerCentre *centre,const char *id,const char *name,UmiDatabaseProvider provider,const char *endpoint,const char *database_name,const char *secret_reference,int read_only);
UmiStatus umi_studio_database_connection_activate(UmiStudioDatabaseExplorerCentre *centre,const char *id);
#endif
