/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/workspace_main.c
 *
 * PURPOSE:
 *   Inspect an indexed Studio workspace, discovered projects and optional text
 *   search from a native command without starting GTK4.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/workspace.h"
#include "umicom/studio/search.h"

#include <stdio.h>
#include <string.h>

static UmiStatus print_match(const UmiSearchMatch *match, void *user_data)
{
    (void)user_data;
    (void)printf("%s:%zu:%zu: %s\n",
                 match->path,
                 match->line,
                 match->column,
                 match->preview);
    return UMI_STATUS_OK;
}

int main(int argc, char **argv)
{
    UmiStudioServices *services = NULL;
    UmiStudioWorkspaceSnapshot snapshot;
    UmiStatus status;
    const char *root = argc >= 2 ? argv[1] : ".";
    const char *query = NULL;
    size_t index;
    int trusted = 0;

    for (index = 2U; index < (size_t)argc; ++index) {
        if (strcmp(argv[index], "--trusted") == 0) trusted = 1;
        else if (strcmp(argv[index], "--search") == 0 && index + 1U < (size_t)argc) {
            query = argv[++index];
        }
    }

    status = umi_studio_services_create(NULL, NULL, &services);
    if (status == UMI_STATUS_OK) {
        status = umi_studio_workspace_open(services, root, trusted, 0);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_studio_workspace_snapshot(services, &snapshot);
    }
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Workspace failed: %s\n", umi_status_text(status));
        umi_studio_services_destroy(services);
        return 1;
    }

    (void)printf("Workspace: %s\n", snapshot.graph.root);
    (void)printf("Trusted: %s\n", snapshot.graph.trusted ? "yes" : "no");
    (void)printf("Projects: %zu\n", snapshot.graph.project_count);
    (void)printf("Indexed files: %zu\n", snapshot.files.files);
    for (index = 0U; index < snapshot.graph.project_count; ++index) {
        UmiWorkspaceProjectSnapshot project;
        if (umi_studio_workspace_project_at(services, index, &project) ==
            UMI_STATUS_OK) {
            (void)printf("Project[%zu]: %s (%s)\n",
                         index,
                         project.display_name,
                         project.root);
        }
    }

    if (query != NULL) {
        UmiSearchRequest request = umi_search_request_default(query);
        UmiSearchStats stats;
        request.maximum_results = 50U;
        status = umi_studio_search_text(services,
                                        &request,
                                        print_match,
                                        NULL,
                                        &stats);
        if (status == UMI_STATUS_OK) {
            (void)printf("Matches: %zu; files searched: %zu\n",
                         stats.matches,
                         stats.files_searched);
        }
    }

    umi_studio_services_destroy(services);
    return status == UMI_STATUS_OK ? 0 : 1;
}
