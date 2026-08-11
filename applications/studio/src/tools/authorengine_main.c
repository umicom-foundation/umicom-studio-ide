/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/authorengine_main.c
 *
 * PURPOSE:
 *   Validate the process-boundary configuration Studio uses for Umicom AuthorEngine.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This native command exercises the same Framework-backed service that future Studio GUI views will call.
 */

#include "umicom/studio/authorengine.h"

#include <stdio.h>

int main(int argc, char **argv)
{
    UmiAiAuthorEngineConfig config;
    const char *executable = argc > 1 ? argv[1] : "uaengine";
    const char *workspace = argc > 2 ? argv[2] : ".";
    UmiStatus status = umi_studio_authorengine_config(executable, workspace, &config);
    if (status == UMI_STATUS_OK) {
        printf("AuthorEngine executable: %s\nWorkspace: %s\nProvider: %s\n",
               config.executable, config.workspace, config.provider);
    }
    return status == UMI_STATUS_OK ? 0 : 1;
}
