/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/workspace-productivity/tests/test_document_set.c
 *
 * PURPOSE:
 *   Exercise one workspace/editor productivity behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is part of Studio workspace/editor productivity. The implementation
 * uses bounded C23 data structures and explicit ownership so the behaviour is
 * easy to inspect, test and later connect to the main Studio workbench.
 */

#include <assert.h>
#include "umicom/studio/document_set.h"
int main(void){
    UmiStudioDocumentSet s;
    umi_studio_document_set_init(&s);
    assert(umi_studio_document_set_open(&s,"a.c","C",false)==UMI_STATUS_OK);
    assert(umi_studio_document_set_open(&s,"b.c","C",true)==UMI_STATUS_OK);
    assert(s.count==2U);
    assert(umi_studio_document_set_active(&s)!=NULL);
    return 0;
}
