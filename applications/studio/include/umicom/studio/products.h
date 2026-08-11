/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/products.h
 *
 * PURPOSE:
 *   Expose Studio product-catalogue queries for independent Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio is the reference consumer. These adapters call public Framework ABI, SDK and product APIs rather than duplicating those mechanisms inside the IDE.
 */
#ifndef UMICOM_STUDIO_PRODUCTS_H
#define UMICOM_STUDIO_PRODUCTS_H
#include "umicom/studio/compatibility_platform.h"
const UmiProductProfile *umi_studio_product_find(const char *product_id);
UmiStatus umi_studio_product_report(const UmiProductProfile *profile,char *buffer,size_t capacity);
#endif

