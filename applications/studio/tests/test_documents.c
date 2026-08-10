/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_documents.c
 *
 * PURPOSE:
 *   Verify Studio document use cases, session tracking, atomic save and
 *   recovery checkpoint restoration through the Framework services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/documents.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    UmiStudioServices *services = NULL;
    UmiDocumentId document_id = 0U;
    UmiDocumentId recovered_id = 0U;
    UmiDocumentSnapshot snapshot;
    char temp_root[UMI_PATH_CAPACITY];
    char test_root[UMI_PATH_CAPACITY];
    char path[UMI_PATH_CAPACITY];
    char *text = NULL;

    (void)umi_fs_remove_tree(".umicom");
    assert(umi_fs_temp_directory(temp_root, sizeof(temp_root)) == UMI_STATUS_OK);
    assert(umi_fs_join(test_root,
                       sizeof(test_root),
                       temp_root,
                       "umicom-studio-documents-test") == UMI_STATUS_OK);
    (void)umi_fs_remove_tree(test_root);
    assert(umi_fs_make_directories(test_root) == UMI_STATUS_OK);
    assert(umi_fs_join(path,
                       sizeof(path),
                       test_root,
                       "document.c") == UMI_STATUS_OK);

    assert(umi_studio_services_create(NULL, NULL, &services) == UMI_STATUS_OK);
    assert(umi_studio_documents_new(services,
                                    "document.c",
                                    &document_id) == UMI_STATUS_OK);
    assert(umi_studio_documents_replace(services,
                                        document_id,
                                        "int value = 7;\n",
                                        15U) == UMI_STATUS_OK);
    assert(umi_studio_documents_checkpoint(services,
                                           document_id) == UMI_STATUS_OK);
    assert(umi_recovery_manager_exists(
        umi_studio_services_recovery(services),
        document_id
    ));

    assert(umi_studio_documents_restore_recovery(services,
                                                 document_id,
                                                 &recovered_id) ==
           UMI_STATUS_OK);
    assert(umi_document_store_copy_text(
        umi_studio_services_documents(services),
        recovered_id,
        &text,
        NULL
    ) == UMI_STATUS_OK);
    assert(strcmp(text, "int value = 7;\n") == 0);
    umi_document_store_free_text(text);

    assert(umi_studio_documents_save_as(services,
                                       document_id,
                                       path) == UMI_STATUS_OK);
    assert(umi_document_store_snapshot(
        umi_studio_services_documents(services),
        document_id,
        &snapshot
    ) == UMI_STATUS_OK);
    assert(!snapshot.dirty);
    assert(!umi_recovery_manager_exists(
        umi_studio_services_recovery(services),
        document_id
    ));

    assert(umi_studio_documents_close(services,
                                     recovered_id,
                                     1) == UMI_STATUS_OK);
    assert(umi_studio_documents_close(services,
                                     document_id,
                                     0) == UMI_STATUS_OK);

    umi_studio_services_destroy(services);
    assert(umi_fs_remove_tree(test_root) == UMI_STATUS_OK);
    assert(umi_fs_remove_tree(".umicom") == UMI_STATUS_OK);
    return 0;
}
