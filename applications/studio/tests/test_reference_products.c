/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_reference_products.c
 *
 * PURPOSE:
 *   Validate reference products behaviour in the reference IDE consumer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This integration test proves Studio consumes the public Framework contract rather than a private duplicate implementation.
 */
#include <assert.h>
#include "umicom/product/product.h"
int main(void){size_t i;for(i=0U;i<umi_reference_product_count();++i){UmiProductValidation v;assert(umi_product_validation_run(umi_reference_product_at(i),&v)==UMI_STATUS_OK);assert(v.passed);}return 0;}
