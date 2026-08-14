/* Umicom Studio IDE | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/database_explorer_queries.h"
UmiStatus umi_studio_database_query_execute(UmiStudioDatabaseExplorerCentre *c,const char *statement,UmiDatabaseQueryResult *out){UmiStatus s;if(c==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_database_query_session_execute(&c->explorer->queries,statement,out);c->revision+=1U;return s;}
UmiStatus umi_studio_database_transaction_begin(UmiStudioDatabaseExplorerCentre *c){UmiStatus s;if(c==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_database_explorer_transaction_begin(&c->explorer->transaction);if(s==UMI_STATUS_OK)c->revision+=1U;return s;}
UmiStatus umi_studio_database_transaction_commit(UmiStudioDatabaseExplorerCentre *c){UmiStatus s;if(c==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_database_explorer_transaction_commit(&c->explorer->transaction);if(s==UMI_STATUS_OK)c->revision+=1U;return s;}
UmiStatus umi_studio_database_transaction_rollback(UmiStudioDatabaseExplorerCentre *c){UmiStatus s;if(c==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_database_explorer_transaction_rollback(&c->explorer->transaction);if(s==UMI_STATUS_OK)c->revision+=1U;return s;}
