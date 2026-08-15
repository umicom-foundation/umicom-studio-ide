/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/appearance_centre.h
 *
 * PURPOSE:
 *   Define Studio's branded appearance composition over the reusable Umicom
 *   Framework appearance catalogue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_STUDIO_APPEARANCE_CENTRE_H
#define UMICOM_STUDIO_APPEARANCE_CENTRE_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_APPEARANCE_SYSTEM "umicom-system"
#define UMI_STUDIO_APPEARANCE_LIGHT "umicom-light"
#define UMI_STUDIO_APPEARANCE_DARK "umicom-dark"
#define UMI_STUDIO_APPEARANCE_HIGH_CONTRAST "umicom-high-contrast"
#define UMI_STUDIO_APPEARANCE_SESSION_KEY "studio.ui.appearance-profile.v1"

UmiStatus umi_studio_appearance_register(UmiUiWorkbench *workbench);
UmiStatus umi_studio_appearance_activate(UmiUiWorkbench *workbench,
                                         const char *profile_id);
UmiStatus umi_studio_appearance_restore(UmiUiWorkbench *workbench,
                                        UmiSessionStore *session);
UmiStatus umi_studio_appearance_save(UmiUiWorkbench *workbench,
                                     UmiSessionStore *session);
UmiStatus umi_studio_appearance_set_brand_resources(
    UmiUiWorkbench *workbench,
    const char *logo_path,
    const char *icon_path);

#ifdef __cplusplus
}
#endif

#endif
