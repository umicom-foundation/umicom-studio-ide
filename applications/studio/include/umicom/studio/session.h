/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/session.h
 *
 * PURPOSE:
 *   Define Studio-specific session keys and narrow helpers over the Framework
 *   session and recovery services without making GTK widgets authoritative.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_SESSION_H
#define UMICOM_STUDIO_SESSION_H

#include "umicom/studio/services.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_SESSION_DEFAULT_PATH ".umicom/studio.session"
#define UMI_STUDIO_RECOVERY_DEFAULT_ROOT ".umicom/recovery"

const char *umi_studio_session_default_path(void);
const char *umi_studio_recovery_default_root(void);
UmiStatus umi_studio_session_set_workspace(UmiStudioServices *services,
                                           const char *workspace_root);
UmiStatus umi_studio_session_set_active_document(
    UmiStudioServices *services,
    UmiDocumentId document_id
);
UmiStatus umi_studio_session_remember_document(
    UmiStudioServices *services,
    UmiDocumentId document_id,
    const char *path
);
UmiStatus umi_studio_session_forget_document(
    UmiStudioServices *services,
    UmiDocumentId document_id
);
UmiStatus umi_studio_session_save(UmiStudioServices *services);
UmiStatus umi_studio_session_reload(UmiStudioServices *services,
                                    int *out_loaded);

#ifdef __cplusplus
}
#endif

#endif
