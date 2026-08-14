/* Umicom Studio IDE | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_DATABASE_EXPLORER_MIGRATIONS_H
#define UMICOM_STUDIO_DATABASE_EXPLORER_MIGRATIONS_H
#include "umicom/studio/database_explorer_centre.h"
UmiStatus umi_studio_database_migration_register(UmiStudioDatabaseExplorerCentre *centre,uint32_t version,const char *name,const char *checksum);
UmiStatus umi_studio_database_migrations_apply_core(UmiStudioDatabaseExplorerCentre *centre,uint32_t target_version,uint64_t applied_at,size_t *out_applied);
#endif
