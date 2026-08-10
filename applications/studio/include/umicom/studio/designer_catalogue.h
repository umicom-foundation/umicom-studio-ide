/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/designer_catalogue.h
 *
 * PURPOSE:
 *   Expose Studio-friendly catalogue and palette reports over Framework semantic component metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This Studio-facing contract composes reusable Framework capabilities.
 * Generic designer and declarative logic remains owned by Umicom Framework.
 */

#ifndef UMICOM_STUDIO_DESIGNER_CATALOGUE_H
#define UMICOM_STUDIO_DESIGNER_CATALOGUE_H
#include "umicom/studio/declarative.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_studio_designer_catalogue_report(UmiStudioDeclarative *service,const char *category_filter,char *out_text,size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
