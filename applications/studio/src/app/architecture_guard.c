/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/architecture_guard.c
 *
 * PURPOSE:
 *   Check reference product profiles remain valid and compatible with public Framework capabilities.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio is the reference consumer. These adapters call public Framework ABI, SDK and product APIs rather than duplicating those mechanisms inside the IDE.
 */
#include "umicom/studio/architecture_guard.h"
UmiStatus umi_studio_architecture_guard_run(UmiStudioArchitectureGuard *o){size_t i;if(o==NULL)return UMI_STATUS_INVALID_ARGUMENT;o->profiles_checked=0U;o->invalid_profiles=0U;o->incompatible_profiles=0U;for(i=0U;i<umi_reference_product_count();++i){UmiProductValidation v;UmiProductCompatibility c;const UmiProductProfile *p=umi_reference_product_at(i);++o->profiles_checked;(void)umi_product_validation_run(p,&v);if(!v.passed)++o->invalid_profiles;(void)umi_product_compatibility_check(p,&c);if(!c.compatible)++o->incompatible_profiles;}o->passed=o->invalid_profiles==0U&&o->incompatible_profiles==0U;return UMI_STATUS_OK;}
