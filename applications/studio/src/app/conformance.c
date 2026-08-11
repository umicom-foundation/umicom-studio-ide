/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/conformance.c
 *
 * PURPOSE:
 *   Run Studio conformance checks across all canonical reference products.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio is the reference consumer. These adapters call public Framework ABI, SDK and product APIs rather than duplicating those mechanisms inside the IDE.
 */
#include "umicom/studio/conformance.h"
UmiStatus umi_studio_conformance_run(UmiStudioCompatibilityPlatform *p,UmiStudioConformanceResult *o){size_t i;if(p==NULL||o==NULL)return UMI_STATUS_INVALID_ARGUMENT;o->products_checked=0U;o->products_compatible=0U;o->abi_ok=UMICOM_FRAMEWORK_ABI_VERSION>=2U;for(i=0U;i<umi_studio_compatibility_product_count(p);++i){UmiProductCompatibility c;const UmiProductProfile *profile=umi_studio_compatibility_product_at(p,i);++o->products_checked;if(umi_product_compatibility_check(profile,&c)==UMI_STATUS_OK&&c.compatible)++o->products_compatible;}o->passed=o->abi_ok&&o->products_checked==o->products_compatible;return UMI_STATUS_OK;}
