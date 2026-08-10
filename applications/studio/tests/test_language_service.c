/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_language_service.c
 *
 * PURPOSE:
 *   Verify Studio Language Server Protocol initialisation, document sync and completion requests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/language.h"

#include <assert.h>

int main(void)
{
    UmiStudioLanguageService *service = NULL;
    UmiStudioLanguageSnapshot snapshot;
    UmiLspPosition position = {0U, 4U};
    int64_t request_id = 0;
    assert(umi_studio_language_service_create("file:///workspace", &service) == UMI_STATUS_OK);
    assert(umi_studio_language_service_initialize(service, 0, &request_id) == UMI_STATUS_OK);
    assert(request_id == 1);
    assert(umi_studio_language_service_open(service,
                                             "file:///workspace/main.c",
                                             "c",
                                             1,
                                             "int main(void) { return 0; }") == UMI_STATUS_OK);
    assert(umi_studio_language_service_completion(service,
                                                   "file:///workspace/main.c",
                                                   position,
                                                   &request_id) == UMI_STATUS_OK);
    assert(request_id == 2);
    assert(umi_studio_language_service_snapshot(service, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.initialized);
    assert(snapshot.open_documents == 1U);
    assert(snapshot.queued_messages == 4U);
    umi_studio_language_service_destroy(service);
    return 0;
}
