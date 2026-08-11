/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_ai_retrieval.c
 *
 * PURPOSE:
 *   Validate Studio's Framework-backed AI and Helix integration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This focused test verifies one product adapter while keeping the provider-neutral implementation inside Framework.
 */



#include <assert.h>
#include "umicom/studio/ai_retrieval.h"
int main(void){UmiAiRetrievalResult r[4];size_t n=umi_studio_ai_retrieve("d","Umicom Framework powers Studio and Trader.","Framework",r,4U);assert(n>0U);return 0;}
