/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/authorengine.h
 *
 * PURPOSE:
 *   Build and validate Studio's provider-neutral AuthorEngine process configuration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio points at AuthorEngine through an executable/workspace contract instead of copying its book-building implementation.
 */

#ifndef APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_AUTHORENGINE_H
#define APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_AUTHORENGINE_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_authorengine_config(const char *executable,
                                         const char *workspace,
                                         UmiAiAuthorEngineConfig *config);

#ifdef __cplusplus
}
#endif

#endif
