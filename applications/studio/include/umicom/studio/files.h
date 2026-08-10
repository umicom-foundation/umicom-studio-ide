/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/files.h
 *
 * PURPOSE:
 *   Provide workspace-scoped Studio file operations that resolve paths through
 *   Framework, enforce workspace trust for mutations and keep the shared index
 *   current after successful changes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_FILES_H
#define UMICOM_STUDIO_FILES_H

#include "umicom/studio/services.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_files_resolve(UmiStudioServices *services,
                                   const char *workspace_path,
                                   char *out_path,
                                   size_t capacity);
UmiStatus umi_studio_files_read_text(UmiStudioServices *services,
                                     const char *workspace_path,
                                     char **out_text,
                                     size_t *out_size);
UmiStatus umi_studio_files_write_text(UmiStudioServices *services,
                                      const char *workspace_path,
                                      const char *text);
UmiStatus umi_studio_files_append_text(UmiStudioServices *services,
                                       const char *workspace_path,
                                       const char *text);
UmiStatus umi_studio_files_create_directory(UmiStudioServices *services,
                                            const char *workspace_path);
UmiStatus umi_studio_files_rename(UmiStudioServices *services,
                                  const char *source_workspace_path,
                                  const char *destination_workspace_path);
UmiStatus umi_studio_files_remove(UmiStudioServices *services,
                                  const char *workspace_path);

#ifdef __cplusplus
}
#endif

#endif
