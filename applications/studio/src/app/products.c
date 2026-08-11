/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/products.c
 *
 * PURPOSE:
 *   Format and query the canonical Umicom reference-product catalogue from Studio.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio is the reference consumer. These adapters call public Framework ABI, SDK and product APIs rather than duplicating those mechanisms inside the IDE.
 */
#include "umicom/studio/products.h"
#include <stdio.h>
const UmiProductProfile *umi_studio_product_find(const char *id){return umi_reference_product_find(id);}
UmiStatus umi_studio_product_report(const UmiProductProfile *p,char *b,size_t cap){UmiProductCompatibility c;int n;if(p==NULL||b==NULL||cap==0U)return UMI_STATUS_INVALID_ARGUMENT;if(umi_product_compatibility_check(p,&c)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;n=snprintf(b,cap,"Product: %s\nID: %s\nCapabilities: %zu\nMissing: %zu\nCompatible: %s\n",p->descriptor->branding.display_name,p->descriptor->product_id,p->capability_count,c.missing_capabilities,c.compatible?"yes":"no");return n<0||(size_t)n>=cap?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_OK;}
