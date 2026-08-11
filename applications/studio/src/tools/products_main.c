/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/products_main.c
 *
 * PURPOSE:
 *   List canonical Umicom reference products from Studio.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This command exposes one compatibility check in a form that is easy to run from PowerShell, CI, or the future Studio workbench UI.
 */
#include <stdio.h>
#include "umicom/product/product.h"
int main(void){size_t i;for(i=0U;i<umi_reference_product_count();++i){const UmiProductProfile *p=umi_reference_product_at(i);printf("%s | %s | capabilities=%zu | frontends=%zu\n",p->descriptor->branding.display_name,p->descriptor->product_id,p->capability_count,p->frontend_count);}return 0;}
